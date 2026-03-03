#include "tools/ui_templates.h"
#include <algorithm>
#include <cctype>

namespace mcdk {

// 辅助：将字符串转为小写下划线风格
static std::string to_snake(const std::string& s) {
    std::string r;
    for (char c : s) {
        if (c == ' ' || c == '-') r += '_';
        else r += (char)std::tolower((unsigned char)c);
    }
    return r;
}

// 辅助：将 snake_case 转为 PascalCase
static std::string to_pascal(const std::string& s) {
    std::string r;
    bool up = true;
    for (char c : s) {
        if (c == '_' || c == '-' || c == ' ') { up = true; continue; }
        r += up ? (char)std::toupper((unsigned char)c) : c;
        up = false;
    }
    return r;
}

// 生成 Python 文件头（注册函数 + 类定义开头）
static std::string py_header(const std::string& mod, const std::string& snake,
                              const std::string& cls, const std::string& ns,
                              const std::string& ui_key) {
    return
        "# -*- coding: utf-8 -*-\n"
        "from mod.client.ui.screenNode import ScreenNode\n"
        "import mod.client.extraClientApi as clientApi\n"
        "\n"
        "def Register" + cls + "():\n"
        "    clientApi.RegisterUI(\"" + mod + "\", \"" + snake + "_" + ui_key + "\", \""
            + mod + ".ui." + cls + "." + cls + "\", \"" + ns + ".main\")\n"
        "\n"
        "class " + cls + "(ScreenNode):\n"
        "    def __init__(self, namespace, name, param):\n"
        "        ScreenNode.__init__(self, namespace, name, param)\n";
}

// ── screen_basic ──────────────────────────────────────
static UiTemplateResult gen_screen_basic(const std::string& ns, const std::string& mod) {
    std::string cls   = to_pascal(mod) + "ScreenNode";
    std::string snake = to_snake(mod);

    std::string json =
        "{\n"
        "  \"namespace\": \"" + ns + "\",\n"
        "  \"main\": {\n"
        "    \"type\": \"panel\", \"size\": [\"100%\", \"100%\"],\n"
        "    \"controls\": [\n"
        "      { \"bg\": { \"type\": \"image\", \"size\": [\"100%\", \"100%\"], \"texture\": \"textures/ui/bg32\", \"alpha\": 0.6, \"layer\": 0 } },\n"
        "      {\n"
        "        \"panel\": {\n"
        "          \"type\": \"panel\", \"size\": [\"80%\", \"80%\"], \"anchor_from\": \"center\", \"anchor_to\": \"center\", \"layer\": 1,\n"
        "          \"controls\": [\n"
        "            {\n"
        "              \"workPanel\": {\n"
        "                \"type\": \"panel\", \"size\": [\"100%\", \"100%\"],\n"
        "                \"controls\": [\n"
        "                  {\n"
        "                    \"topPanel\": {\n"
        "                      \"type\": \"stack_panel\", \"orientation\": \"horizontal\",\n"
        "                      \"size\": [\"100%\", 30], \"anchor_from\": \"top_middle\", \"anchor_to\": \"top_middle\",\n"
        "                      \"controls\": [\n"
        "                        { \"label\": { \"type\": \"label\", \"text\": \"" + ns + "\", \"color\": [1,1,1], \"font_scale_factor\": 1.2, \"size\": [\"fill\", 30] } },\n"
        "                        {\n"
        "                          \"closeBtn@common.button\": {\n"
        "                            \"size\": [20, 20],\n"
        "                            \"$pressed_button_name\": \"button.close\",\n"
        "                            \"controls\": [\n"
        "                              { \"icon\": { \"type\": \"image\", \"size\": [16, 16], \"texture\": \"textures/ui/cancel\" } }\n"
        "                            ]\n"
        "                          }\n"
        "                        }\n"
        "                      ]\n"
        "                    }\n"
        "                  },\n"
        "                  {\n"
        "                    \"content\": {\n"
        "                      \"type\": \"panel\",\n"
        "                      \"size\": [\"100%\", \"100%-30px\"],\n"
        "                      \"anchor_from\": \"bottom_middle\", \"anchor_to\": \"bottom_middle\"\n"
        "                    }\n"
        "                  }\n"
        "                ]\n"
        "              }\n"
        "            }\n"
        "          ]\n"
        "        }\n"
        "      }\n"
        "    ]\n"
        "  }\n"
        "}";

    std::string py = py_header(mod, snake, cls, ns, "screen") +
        "\n"
        "    def Create(self):\n"
        "        self.AddTouchEventHandler(\"/panel/workPanel/topPanel/closeBtn\", self.OnCloseButton)\n"
        "\n"
        "    def OnCloseButton(self, args):\n"
        "        if args[\"TouchEvent\"] == 0:\n"
        "            self.SetScreenVisible(False)\n";

    return {json, py, "\"ui/" + snake + "_screen.json\"", ""};
}

// ── screen_list ───────────────────────────────────────
static UiTemplateResult gen_screen_list(const std::string& ns, const std::string& mod) {
    std::string cls   = to_pascal(mod) + "ListScreenNode";
    std::string snake = to_snake(mod);

    std::string json =
        "{\n"
        "  \"namespace\": \"" + ns + "\",\n"
        "  \"list_item\": {\n"
        "    \"type\": \"panel\", \"size\": [\"100%\", 40],\n"
        "    \"controls\": [\n"
        "      { \"text\": { \"type\": \"label\", \"text\": \"#item_text\", \"color\": [1,1,1], \"size\": [\"100%\", \"100%\"],\n"
        "        \"bindings\": [{ \"binding_name\": \"#item_text\" }] } }\n"
        "    ]\n"
        "  },\n"
        "  \"main\": {\n"
        "    \"type\": \"panel\", \"size\": [\"100%\", \"100%\"],\n"
        "    \"controls\": [\n"
        "      { \"bg\": { \"type\": \"image\", \"size\": [\"100%\", \"100%\"], \"texture\": \"textures/ui/bg32\", \"alpha\": 0.6, \"layer\": 0 } },\n"
        "      {\n"
        "        \"panel\": {\n"
        "          \"type\": \"panel\", \"size\": [\"80%\", \"80%\"], \"anchor_from\": \"center\", \"anchor_to\": \"center\", \"layer\": 1,\n"
        "          \"controls\": [\n"
        "            {\n"
        "              \"scroll@common.scrolling_panel\": {\n"
        "                \"size\": [\"100%\", \"100%\"],\n"
        "                \"$scrolling_content\": \"" + ns + ".list_item\",\n"
        "                \"$scrolling_content_anchor_from\": \"top_left\",\n"
        "                \"$scrolling_content_anchor_to\": \"top_left\"\n"
        "              }\n"
        "            }\n"
        "          ]\n"
        "        }\n"
        "      }\n"
        "    ]\n"
        "  }\n"
        "}";

    std::string py = py_header(mod, snake, cls, ns, "list") +
        "        self._items = []\n"
        "\n"
        "    def Create(self):\n"
        "        pass\n"
        "\n"
        "    def SetItems(self, items):\n"
        "        \"\"\"正向接口：设置列表数据\"\"\"\n"
        "        self._items = items\n"
        "        for i, item in enumerate(items):\n"
        "            ctrl = self.GetBaseUIControl(\"/panel/scroll/scroll_view/stack_panel/list_item[%d]/text\" % i)\n"
        "            if ctrl:\n"
        "                ctrl.asLabel().SetText(item)\n";

    return {json, py, "\"ui/" + snake + "_list.json\"", ""};
}

// ── screen_grid ───────────────────────────────────────
static UiTemplateResult gen_screen_grid(const std::string& ns, const std::string& mod) {
    std::string cls   = to_pascal(mod) + "GridScreenNode";
    std::string snake = to_snake(mod);

    std::string json =
        "{\n"
        "  \"namespace\": \"" + ns + "\",\n"
        "  \"grid_item\": {\n"
        "    \"type\": \"panel\", \"size\": [50, 50],\n"
        "    \"controls\": [\n"
        "      { \"icon\": { \"type\": \"image\", \"size\": [\"100%\", \"100%\"], \"texture\": \"textures/ui/cell_image_normal\" } }\n"
        "    ]\n"
        "  },\n"
        "  \"main\": {\n"
        "    \"type\": \"panel\", \"size\": [\"100%\", \"100%\"],\n"
        "    \"controls\": [\n"
        "      { \"bg\": { \"type\": \"image\", \"size\": [\"100%\", \"100%\"], \"texture\": \"textures/ui/bg32\", \"alpha\": 0.6, \"layer\": 0 } },\n"
        "      {\n"
        "        \"panel\": {\n"
        "          \"type\": \"panel\", \"size\": [\"80%\", \"80%\"], \"anchor_from\": \"center\", \"anchor_to\": \"center\", \"layer\": 1,\n"
        "          \"controls\": [\n"
        "            {\n"
        "              \"grid\": {\n"
        "                \"type\": \"grid\", \"size\": [\"100%\", \"100%\"],\n"
        "                \"grid_dimensions\": [6, 4],\n"
        "                \"collection_name\": \"grid_items\",\n"
        "                \"controls\": [\n"
        "                  { \"item@" + ns + ".grid_item\": {} }\n"
        "                ]\n"
        "              }\n"
        "            }\n"
        "          ]\n"
        "        }\n"
        "      }\n"
        "    ]\n"
        "  }\n"
        "}";

    std::string py = py_header(mod, snake, cls, ns, "grid") +
        "\n"
        "    def Create(self):\n"
        "        pass\n";

    return {json, py, "\"ui/" + snake + "_grid.json\"", ""};
}

// ── screen_form ───────────────────────────────────────
static UiTemplateResult gen_screen_form(const std::string& ns, const std::string& mod) {
    std::string cls   = to_pascal(mod) + "FormScreenNode";
    std::string snake = to_snake(mod);

    std::string json =
        "{\n"
        "  \"namespace\": \"" + ns + "\",\n"
        "  \"main\": {\n"
        "    \"type\": \"panel\", \"size\": [\"100%\", \"100%\"],\n"
        "    \"controls\": [\n"
        "      { \"bg\": { \"type\": \"image\", \"size\": [\"100%\", \"100%\"], \"texture\": \"textures/ui/bg32\", \"alpha\": 0.6, \"layer\": 0 } },\n"
        "      {\n"
        "        \"panel\": {\n"
        "          \"type\": \"panel\", \"size\": [\"60%\", \"50%\"], \"anchor_from\": \"center\", \"anchor_to\": \"center\", \"layer\": 1,\n"
        "          \"controls\": [\n"
        "            {\n"
        "              \"form\": {\n"
        "                \"type\": \"stack_panel\", \"orientation\": \"vertical\", \"size\": [\"100%\", \"100%\"],\n"
        "                \"controls\": [\n"
        "                  { \"title\": { \"type\": \"label\", \"text\": \"" + ns + "\", \"color\": [1,1,1], \"size\": [\"100%\", 30] } },\n"
        "                  {\n"
        "                    \"inputBox\": {\n"
        "                      \"type\": \"edit_box\", \"size\": [\"100%\", 30],\n"
        "                      \"max_length\": 100,\n"
        "                      \"$place_holder_text\": \"请输入...\",\n"
        "                      \"$text_box_name\": \"#input_text\"\n"
        "                    }\n"
        "                  },\n"
        "                  { \"spacer\": { \"type\": \"panel\", \"size\": [\"100%\", 10] } },\n"
        "                  {\n"
        "                    \"confirmBtn@common.button\": {\n"
        "                      \"size\": [\"100%\", 30],\n"
        "                      \"$pressed_button_name\": \"button.confirm\",\n"
        "                      \"controls\": [\n"
        "                        { \"label\": { \"type\": \"label\", \"text\": \"确认\", \"color\": [1,1,1] } }\n"
        "                      ]\n"
        "                    }\n"
        "                  }\n"
        "                ]\n"
        "              }\n"
        "            }\n"
        "          ]\n"
        "        }\n"
        "      }\n"
        "    ]\n"
        "  }\n"
        "}";

    std::string py = py_header(mod, snake, cls, ns, "form") +
        "        self._input_ctrl = None\n"
        "\n"
        "    def Create(self):\n"
        "        self._input_ctrl = self.GetBaseUIControl(\"/panel/form/inputBox\")\n"
        "        self.AddTouchEventHandler(\"/panel/form/confirmBtn\", self.OnConfirm)\n"
        "\n"
        "    def GetInputText(self):\n"
        "        if self._input_ctrl:\n"
        "            return self._input_ctrl.asEditBox().GetEditText()\n"
        "        return \"\"\n"
        "\n"
        "    def OnConfirm(self, args):\n"
        "        if args[\"TouchEvent\"] == 0:\n"
        "            text = self.GetInputText()\n"
        "            # TODO: 处理表单提交\n";

    return {json, py, "\"ui/" + snake + "_form.json\"", ""};
}

// ── screen_tabbed ─────────────────────────────────────
static UiTemplateResult gen_screen_tabbed(const std::string& ns, const std::string& mod) {
    std::string cls   = to_pascal(mod) + "TabbedScreenNode";
    std::string snake = to_snake(mod);

    std::string json =
        "{\n"
        "  \"namespace\": \"" + ns + "\",\n"
        "  \"tab_content_0\": { \"type\": \"panel\", \"size\": [\"100%\", \"100%\"] },\n"
        "  \"tab_content_1\": { \"type\": \"panel\", \"size\": [\"100%\", \"100%\"], \"visible\": false },\n"
        "  \"main\": {\n"
        "    \"type\": \"panel\", \"size\": [\"100%\", \"100%\"],\n"
        "    \"controls\": [\n"
        "      { \"bg\": { \"type\": \"image\", \"size\": [\"100%\", \"100%\"], \"texture\": \"textures/ui/bg32\", \"alpha\": 0.6, \"layer\": 0 } },\n"
        "      {\n"
        "        \"panel\": {\n"
        "          \"type\": \"panel\", \"size\": [\"80%\", \"80%\"], \"anchor_from\": \"center\", \"anchor_to\": \"center\", \"layer\": 1,\n"
        "          \"controls\": [\n"
        "            {\n"
        "              \"tabs\": {\n"
        "                \"type\": \"stack_panel\", \"orientation\": \"horizontal\",\n"
        "                \"size\": [\"100%\", 30], \"anchor_from\": \"top_left\", \"anchor_to\": \"top_left\",\n"
        "                \"controls\": [\n"
        "                  {\n"
        "                    \"tab0@common.button\": {\n"
        "                      \"size\": [\"50%\", 30], \"$pressed_button_name\": \"button.tab0\",\n"
        "                      \"controls\": [{ \"label\": { \"type\": \"label\", \"text\": \"标签1\", \"color\": [1,1,1] } }]\n"
        "                    }\n"
        "                  },\n"
        "                  {\n"
        "                    \"tab1@common.button\": {\n"
        "                      \"size\": [\"50%\", 30], \"$pressed_button_name\": \"button.tab1\",\n"
        "                      \"controls\": [{ \"label\": { \"type\": \"label\", \"text\": \"标签2\", \"color\": [1,1,1] } }]\n"
        "                    }\n"
        "                  }\n"
        "                ]\n"
        "              }\n"
        "            },\n"
        "            {\n"
        "              \"content\": {\n"
        "                \"type\": \"panel\", \"size\": [\"100%\", \"100%-30px\"],\n"
        "                \"anchor_from\": \"bottom_middle\", \"anchor_to\": \"bottom_middle\",\n"
        "                \"controls\": [\n"
        "                  { \"page0@" + ns + ".tab_content_0\": {} },\n"
        "                  { \"page1@" + ns + ".tab_content_1\": {} }\n"
        "                ]\n"
        "              }\n"
        "            }\n"
        "          ]\n"
        "        }\n"
        "      }\n"
        "    ]\n"
        "  }\n"
        "}";

    std::string py = py_header(mod, snake, cls, ns, "tabbed") +
        "        self._pages = []\n"
        "        self._current_tab = 0\n"
        "\n"
        "    def Create(self):\n"
        "        self._pages = [\n"
        "            self.GetBaseUIControl(\"/panel/content/page0\"),\n"
        "            self.GetBaseUIControl(\"/panel/content/page1\"),\n"
        "        ]\n"
        "        self.AddTouchEventHandler(\"/panel/tabs/tab0\", self.OnTab0)\n"
        "        self.AddTouchEventHandler(\"/panel/tabs/tab1\", self.OnTab1)\n"
        "\n"
        "    def _switch_tab(self, idx):\n"
        "        for i, page in enumerate(self._pages):\n"
        "            if page:\n"
        "                page.SetVisible(i == idx)\n"
        "        self._current_tab = idx\n"
        "\n"
        "    def OnTab0(self, args):\n"
        "        if args[\"TouchEvent\"] == 0:\n"
        "            self._switch_tab(0)\n"
        "\n"
        "    def OnTab1(self, args):\n"
        "        if args[\"TouchEvent\"] == 0:\n"
        "            self._switch_tab(1)\n";

    return {json, py, "\"ui/" + snake + "_tabbed.json\"", ""};
}

// ── hud_overlay ───────────────────────────────────────
static UiTemplateResult gen_hud_overlay(const std::string& ns, const std::string& mod) {
    std::string cls   = to_pascal(mod) + "HudNode";
    std::string snake = to_snake(mod);

    std::string json =
        "{\n"
        "  \"namespace\": \"" + ns + "\",\n"
        "  \"main\": {\n"
        "    \"type\": \"panel\", \"size\": [\"100%\", \"100%\"],\n"
        "    \"is_modal\": false,\n"
        "    \"should_steal_mouse\": false,\n"
        "    \"absorbs_input\": false,\n"
        "    \"always_accepts_input\": false,\n"
        "    \"controls\": [\n"
        "      {\n"
        "        \"hud\": {\n"
        "          \"type\": \"panel\", \"size\": [\"100%\", \"100%\"], \"layer\": 10,\n"
        "          \"controls\": [\n"
        "            {\n"
        "              \"info_label\": {\n"
        "                \"type\": \"label\", \"text\": \"\", \"color\": [1,1,1],\n"
        "                \"size\": [\"100%\", 20],\n"
        "                \"anchor_from\": \"top_left\", \"anchor_to\": \"top_left\",\n"
        "                \"offset\": [5, 5]\n"
        "              }\n"
        "            }\n"
        "          ]\n"
        "        }\n"
        "      }\n"
        "    ]\n"
        "  }\n"
        "}";

    std::string py = py_header(mod, snake, cls, ns, "hud") +
        "        self._info_ctrl = None\n"
        "\n"
        "    def Create(self):\n"
        "        self._info_ctrl = self.GetBaseUIControl(\"/hud/info_label\")\n"
        "\n"
        "    def SetInfoText(self, text):\n"
        "        if self._info_ctrl:\n"
        "            self._info_ctrl.asLabel().SetText(text)\n";

    return {json, py, "\"ui/" + snake + "_hud.json\"", ""};
}

// ── widget_button ─────────────────────────────────────
static UiTemplateResult gen_widget_button(const std::string& ns, const std::string&) {
    std::string json =
        "{\n"
        "  \"namespace\": \"" + ns + "\",\n"
        "  \"my_button@common.button\": {\n"
        "    \"size\": [120, 30],\n"
        "    \"$pressed_button_name\": \"button.my_action\",\n"
        "    \"controls\": [\n"
        "      { \"default@netease_editor_template_namespace.default\": { \"$default_texture\": \"textures/ui/button_borderless_light\" } },\n"
        "      { \"hover@netease_editor_template_namespace.hover\":   { \"$hover_texture\":   \"textures/ui/button_borderless_lighthover\" } },\n"
        "      { \"pressed@netease_editor_template_namespace.pressed\":{ \"$pressed_texture\": \"textures/ui/button_borderless_lightpressed\" } },\n"
        "      { \"label\": { \"type\": \"label\", \"text\": \"按钮\", \"color\": [1,1,1], \"layer\": 3 } }\n"
        "    ]\n"
        "  }\n"
        "}";
    return {json, "", "", ""};
}

// ── widget_progress ───────────────────────────────────
static UiTemplateResult gen_widget_progress(const std::string& ns, const std::string&) {
    std::string json =
        "{\n"
        "  \"namespace\": \"" + ns + "\",\n"
        "  \"my_progress_bar\": {\n"
        "    \"type\": \"panel\", \"size\": [200, 20],\n"
        "    \"controls\": [\n"
        "      {\n"
        "        \"empty@netease_editor_template_namespace.empty_progress_bar\": {\n"
        "          \"$progress_bar_empty_texture\": \"textures/ui/progress_bar_empty\",\n"
        "          \"size\": [\"100%\", \"100%\"]\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"filled@netease_editor_template_namespace.filled_progress_bar\": {\n"
        "          \"$progress_bar_filled_texture\": \"textures/ui/progress_bar_filled\",\n"
        "          \"$clip_direction\": \"left\",\n"
        "          \"$clip_ratio\": 0.5,\n"
        "          \"size\": [\"100%\", \"100%\"]\n"
        "        }\n"
        "      }\n"
        "    ]\n"
        "  }\n"
        "}";
    return {json, "", "", ""};
}

// ── 入口 ──────────────────────────────────────────────
UiTemplateResult generate_ui_fullstack(const std::string& template_type,
                                        const std::string& ns,
                                        const std::string& mod_name) {
    if (template_type == "screen_basic")    return gen_screen_basic(ns, mod_name);
    if (template_type == "screen_list")     return gen_screen_list(ns, mod_name);
    if (template_type == "screen_grid")     return gen_screen_grid(ns, mod_name);
    if (template_type == "screen_form")     return gen_screen_form(ns, mod_name);
    if (template_type == "screen_tabbed")   return gen_screen_tabbed(ns, mod_name);
    if (template_type == "hud_overlay")     return gen_hud_overlay(ns, mod_name);
    if (template_type == "widget_button")   return gen_widget_button(ns, mod_name);
    if (template_type == "widget_progress") return gen_widget_progress(ns, mod_name);
    return {"", "", "",
        "unknown template_type: " + template_type +
        ". Valid: screen_basic, screen_list, screen_grid, screen_form, "
        "screen_tabbed, hud_overlay, widget_button, widget_progress"};
}

} // namespace mcdk
