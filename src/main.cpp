#include "search/search_service.h"
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>
#include <iostream>
#include <string>
#include <fstream>
#include <climits>
#include <filesystem>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

static std::string get_exe_dir() {
    namespace fs = std::filesystem;
#ifdef _WIN32
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return fs::path(buf).parent_path().string();
#else
    return fs::canonical("/proc/self/exe").parent_path().string();
#endif
}

// 通用搜索工具处理函数
using SearchFn = std::vector<mcdk::SearchResult>(mcdk::SearchService::*)(const std::string&, int) const;

static mcp::json handle_search(mcdk::SearchService& svc, SearchFn fn, const mcp::json& params) {
    std::string keyword = params.value("keyword", "");
    if (keyword.empty()) {
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "keyword is required");
    }
    int top_k = params.contains("top_k") && !params["top_k"].is_null() ? params["top_k"].get<int>() : -1;

    auto results = (svc.*fn)(keyword, top_k);

    mcp::json content_arr = mcp::json::array();
    for (const auto& r : results) {
        content_arr.push_back({
            {"type", "text"}, {"text", r.fragment->content},
            {"file", r.fragment->file}, {"line_start", r.fragment->line_start},
            {"line_end", r.fragment->line_end}, {"score", r.score}
        });
    }
    return {{"content", content_arr}};
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    std::string exe_dir = get_exe_dir();
    std::string dicts_dir     = exe_dir + "/dicts";
    std::string knowledge_dir = exe_dir + "/knowledge";

    std::cout << "[MCDK] dicts: " << dicts_dir << std::endl;
    std::cout << "[MCDK] knowledge: " << knowledge_dir << std::endl;

    mcdk::SearchService search_svc(dicts_dir, knowledge_dir);

    mcp::server::configuration conf;
    conf.host    = "127.0.0.1";
    conf.port    = 18766;
    conf.name    = "mcdk-assistant";
    conf.version = "0.1.0";

    mcp::server srv(conf);

    // 注册3个分类搜索工具
    struct ToolDef {
        const char* name;
        const char* desc;
        SearchFn    fn;
    };
    ToolDef tools[] = {
        {"search_api",           "搜索 ModAPI 接口文档", &mcdk::SearchService::search_api},
        {"search_event",         "搜索 ModAPI 事件文档", &mcdk::SearchService::search_event},
        {"search_enum",          "搜索 ModAPI 枚举值文档", &mcdk::SearchService::search_enum},
        {"search_all",           "搜索全部 ModAPI 文档（接口+事件+枚举值）", &mcdk::SearchService::search_all},
        {"search_wiki",          "Search Bedrock Wiki documentation (English keywords)", &mcdk::SearchService::search_wiki},
        {"search_qumod",         "搜索 QuModLibs 框架库文档（QuMod是网易流行的热门框架库，当用户使用QuMod开发时应优先查找此处功能/设计规范），注意：文档本身并不全面，当用户项目中存在QuModLibs包时还应直接分析其源代码", &mcdk::SearchService::search_qumod},
        {"search_netease_guide", "搜索网易MC独占的教学资料内容（不包含国际版通用内容）", &mcdk::SearchService::search_netease_guide},
    };

    // search_game_assets 单独注册（有额外 scope 参数，返回结构不同）
    {
        auto tool = mcp::tool_builder("search_game_assets")
            .with_description(
                "模糊搜索原版 Minecraft 游戏资产文件（行为包/资源包），同时匹配文件路径名与文件内容。"
                "scope: 0=搜索全部资产, 1=仅搜索行为包(behavior_packs), 2=仅搜索资源包(resource_packs)")
            .with_string_param("keyword", "搜索关键词（支持文件名片段或文件内容关键词）", true)
            .with_number_param("scope", "搜索范围：0=全部（默认），1=仅行为包，2=仅资源包", false)
            .with_number_param("top_k", "返回结果数量上限，默认返回全部匹配", false)
            .with_read_only_hint(true)
            .with_idempotent_hint(true)
            .build();

        srv.register_tool(tool,
            [&search_svc](const mcp::json& params, const std::string&) -> mcp::json {
                std::string keyword = params.value("keyword", "");
                if (keyword.empty())
                    throw mcp::mcp_exception(mcp::error_code::invalid_params, "keyword is required");
                int scope  = params.contains("scope")  && !params["scope"].is_null()  ? params["scope"].get<int>()  : 0;
                int top_k  = params.contains("top_k")  && !params["top_k"].is_null()  ? params["top_k"].get<int>()  : -1;

                auto results = search_svc.search_game_assets(keyword, scope, top_k);

                mcp::json content_arr = mcp::json::array();
                for (const auto& r : results) {
                    content_arr.push_back({
                        {"type",     "text"},
                        {"text",     r.snippet},
                        {"file",     r.rel_path},
                        {"score",    r.score}
                    });
                }
                return {{"content", content_arr}};
            }
        );
    }

    for (auto& td : tools) {
        auto tool = mcp::tool_builder(td.name)
            .with_description(td.desc)
            .with_string_param("keyword", "搜索关键词", true)
            .with_number_param("top_k", "返回结果数量上限，默认返回全部", false)
            .with_read_only_hint(true)
            .with_idempotent_hint(true)
            .build();

        auto fn = td.fn;
        srv.register_tool(tool,
            [&search_svc, fn](const mcp::json& params, const std::string&) -> mcp::json {
                return handle_search(search_svc, fn, params);
            }
        );
    }

    // 读取 knowledge 目录下指定文件内容（支持行范围）
    auto read_tool = mcp::tool_builder("read_knowledge")
        .with_description("读取 knowledge 目录下的指定文件内容，搜索结果中的 file 字段可直接作为 path 参数传入")
        .with_string_param("path", "文件相对路径（相对于 knowledge 目录），如 BedrockWiki/items/items-intro.md", true)
        .with_number_param("line_start", "起始行号（1-based），不传则从第1行开始", false)
        .with_number_param("line_end", "结束行号（1-based，含），不传则读到文件末尾", false)
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .build();

    srv.register_tool(read_tool,
        [&knowledge_dir](const mcp::json& params, const std::string&) -> mcp::json {
            std::string rel_path = params.value("path", "");
            if (rel_path.empty())
                throw mcp::mcp_exception(mcp::error_code::invalid_params, "path is required");

            // 安全检查：禁止路径穿越
            if (rel_path.find("..") != std::string::npos)
                throw mcp::mcp_exception(mcp::error_code::invalid_params, "path must not contain '..'");

            std::filesystem::path full_path = std::filesystem::path(knowledge_dir) / std::filesystem::u8path(rel_path);
            std::ifstream ifs(full_path);
            if (!ifs.is_open())
                throw mcp::mcp_exception(mcp::error_code::invalid_params, "file not found: " + rel_path);

            int line_start = params.contains("line_start") && !params["line_start"].is_null()
                ? params["line_start"].get<int>() : 1;
            int line_end = params.contains("line_end") && !params["line_end"].is_null()
                ? params["line_end"].get<int>() : INT_MAX;

            if (line_start < 1) line_start = 1;
            if (line_end < line_start) line_end = line_start;

            std::string result;
            std::string line;
            int cur = 0;
            while (std::getline(ifs, line)) {
                ++cur;
                if (cur < line_start) continue;
                if (cur > line_end) break;
                result += line;
                result += '\n';
            }

            return {{"content", mcp::json::array({{{"type","text"},{"text", result},
                {"file", rel_path}, {"line_start", line_start},
                {"line_end", std::min(cur, line_end)}, {"total_lines", cur}}})}};
        }
    );

    // 列出 knowledge 目录下的文件和文件夹
    auto list_tool = mcp::tool_builder("list_knowledge")
        .with_description("列出 knowledge 目录下指定路径的文件和文件夹列表")
        .with_string_param("path", "相对路径（相对于 knowledge 目录），默认为根目录", false)
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .build();

    srv.register_tool(list_tool,
        [&knowledge_dir](const mcp::json& params, const std::string&) -> mcp::json {
            namespace fs = std::filesystem;
            std::string rel = params.value("path", "");
            if (rel.find("..") != std::string::npos)
                throw mcp::mcp_exception(mcp::error_code::invalid_params, "path must not contain '..'");

            fs::path dir = fs::path(knowledge_dir) / (rel.empty() ? fs::path() : fs::u8path(rel));
            if (!fs::exists(dir) || !fs::is_directory(dir))
                throw mcp::mcp_exception(mcp::error_code::invalid_params, "directory not found: " + rel);

            mcp::json dirs = mcp::json::array();
            mcp::json files = mcp::json::array();
            for (const auto& entry : fs::directory_iterator(dir)) {
                auto name = entry.path().filename().u8string();
                std::string name_str(reinterpret_cast<const char*>(name.data()), name.size());
                if (entry.is_directory()) dirs.push_back(name_str);
                else files.push_back(name_str);
            }

            std::string text = "目录: " + (rel.empty() ? "/" : rel) + "\n";
            for (const auto& d : dirs) text += "[DIR]  " + d.get<std::string>() + "\n";
            for (const auto& f : files) text += "       " + f.get<std::string>() + "\n";

            return {{"content", mcp::json::array({{{"type","text"},{"text", text}}})}};
        }
    );

    // 网易版与国际版差异说明工具
    static const char* NETEASE_DIFF_TEXT =
        "网易中国版基岩版与国际版基岩版的关键差异（当Wiki文档提到以下国际版路径时，请自动替换为网易版路径）：\n"
        "\n"
        "[行为包目录映射]\n"
        "国际版 BP/items/        -> 网易版 BP/netease_items_beh/\n"
        "国际版 BP/blocks/       -> 网易版 BP/netease_blocks/\n"
        "国际版 BP/biomes/       -> 网易版 BP/netease_biomes/\n"
        "国际版 BP/features/     -> 网易版 BP/netease_features/\n"
        "国际版 BP/feature_rules/-> 网易版 BP/netease_feature_rules/\n"
        "国际版 BP/recipes/      -> 网易版 BP/netease_recipes/\n"
        "\n"
        "[资源包目录映射]\n"
        "国际版 RP/items/        -> 网易版 RP/netease_items_res/\n"
        "\n"
        "[脚本系统]\n"
        "国际版使用 Script API（JavaScript/TypeScript），网易版使用 Mod SDK（Python2）。\n"
        "Wiki中涉及 Script API / @minecraft/server 的内容不适用于网易版，网易版应使用 mod.server.extraServerApi / mod.client.extraClientApi。\n"
        "\n"
        "[开发框架]\n"
        "如果项目中存在 QuModLibs 库，优先使用 QuModLibs 的框架模块来简化网易Mod开发。\n"
        "\n"
        "[版本兼容性]\n"
        "网易版本号通常比国际版慢几个版本，编写JSON时应尽可能使用兼容性最高的 format_version，避免使用国际版最新特性。\n"
        "\n"
        "[JSON UI]\n"
        "JSON UI文件与网易版存在少许差异，涉及相关功能时请调用 get_netease_jsonui 工具获取网易版JSON UI控件库定义。";

    auto diff_tool = mcp::tool_builder("get_netease_diff")
        .with_description("获取网易版与国际版基岩版之间的差异说明")
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .build();

    srv.register_tool(diff_tool, [](const mcp::json&, const std::string&) -> mcp::json {
        return {{"content", mcp::json::array({{{"type","text"},{"text", NETEASE_DIFF_TEXT}}})}};
    });

    // 网易版 JSON UI 控件库定义工具
    static const char* NETEASE_JSONUI_TEXT = R"(网易版JSON UI内置组件库（namespace: netease_editor_template_namespace）
可以被自定义UI控件直接引用继承使用，也可以覆写属性扩展功能
以下是完整的组件定义：
{
  "namespace": "netease_editor_template_namespace",

  "netease_editor_template_image": {
    "type": "image",
    "size": [ "50%", "50%" ],
    "texture": "textures/netease/common/image/default",
    "uv_size": [ 107, 107 ]
  },

  "panel": {
    "type": "panel",
    "size": [ 100, 100 ]
  },

  "image": {
    "layer": 1,
    "size": [ 100, 100 ],
    "texture": "textures/netease/common/image/default",
    "type": "image",
    "clip_direction": "left",
    "clip_ratio": 0.0
  },

  "netease_editor_root_panel": {
    "type": "panel",
    "anchor_from": "top_left",
    "anchor_to": "top_left",
    "size": [ "100%", "100%" ],
    "layer": 1
  },

  "default": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": "$texture_layer",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$default_texture",
    "alpha": "$control_alpha",
    "type": "image"
  },

  "hover": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": "$texture_layer",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$hover_texture",
    "alpha": "$control_alpha",
    "type": "image"
  },

  "pressed": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": "$texture_layer",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$pressed_texture",
    "alpha": "$control_alpha",
    "type": "image"
  },

  "button_label": {
    "color": "$label_color",
    "font_scale_factor": "$label_font_scale_factor",
    "font_size": "$label_font_size",
    "font_type": "default",
    "layer": "$label_layer",
    "max_size": [ "100%", "100%" ],
    "offset": "$label_offset",
    "text": "$label_text",
    "text_alignment": "center",
    "alpha": "$control_alpha",
    "type": "label"
  },

  "scroll_box_image": {
    "is_new_nine_slice": "$is_box_nine_slice",
    "nine_slice_buttom": "$box_nine_slice_buttom",
    "nine_slice_left": "$box_nine_slice_left",
    "nine_slice_right": "$box_nine_slice_right",
    "nine_slice_top": "$box_nine_slice_top",
    "nineslice_size": "$box_nineslice_size",
    "texture": "$scroll_box_texture",
    "type": "image"
  },

  "scroll_track_image": {
    "is_new_nine_slice": "$is_track_nine_slice",
    "layer": 3,
    "nine_slice_buttom": "$track_nine_slice_buttom",
    "nine_slice_left": "$track_nine_slice_left",
    "nine_slice_right": "$track_nine_slice_right",
    "nine_slice_top": "$track_nine_slice_top",
    "nineslice_size": "$track_nineslice_size",
    "size": [ 3, "100%" ],
    "texture": "$scroll_track_texture",
    "type": "image"
  },

  "scroll_background_image": {
    "is_new_nine_slice": "$is_background_nine_slice",
    "nine_slice_buttom": "$background_nine_slice_buttom",
    "nine_slice_left": "$background_nine_slice_left",
    "nine_slice_right": "$background_nine_slice_right",
    "nine_slice_top": "$background_nine_slice_top",
    "nineslice_size": "$background_nineslice_size",
    "texture": "$scroll_background_texture",
    "type": "image"
  },

  "edit_box_background_default": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$edit_box_default_texture",
    "type": "image"
  },

  "edit_box_background_hover": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$edit_box_hover_texture",
    "type": "image"
  },

  "empty_progress_bar": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": 1,
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$progress_bar_empty_texture",
    "type": "image"
  },

  "filled_progress_bar": {
    "clip_direction": "$clip_direction",
    "clip_pixelperfect": false,
    "clip_ratio": "$clip_ratio",
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": 2,
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$progress_bar_filled_texture",
    "type": "image"
  }
}
)";

    auto jsonui_tool = mcp::tool_builder("get_netease_jsonui")
        .with_description("获取网易版JSON UI内置组件库（netease_editor_template_namespace）的完整定义，包含可通过%引用的预定义控件")
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .build();

    srv.register_tool(jsonui_tool, [](const mcp::json&, const std::string&) -> mcp::json {
        return {{"content", mcp::json::array({{{"type","text"},{"text", NETEASE_JSONUI_TEXT}}})}};
    });

    std::cout << "[MCDK] MCP server starting on " << conf.host << ":" << conf.port << std::endl;
    std::cout << "[MCDK] docs indexed: " << search_svc.doc_count() << std::endl;
    std::cout << "[MCDK] game assets indexed: " << search_svc.game_assets_count() << std::endl;

    srv.start(true);

    return 0;
}
