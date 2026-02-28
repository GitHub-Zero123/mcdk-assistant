# MCDK-ASSISTANT
为MCMOD开发提供MCP相关的功能和工具，帮助AI更好地理解和使用MCP进行开发。

## MCP客户端配置

支持标准MCP客户端接入，以下配置以 `Roo Code` 为例。

```jsonc
{
    // Roo Code MCP Settings
    "mcpServers": {
		"minecraft_mod_assistant": {
			"url": "http://localhost:18766/sse",
			"name": "Minecraft Mod Assistant MCP Server(MCDK)",
			"alwaysAllow": [
				"search_all",
				"search_api",
				"search_enum",
				"search_event"
			],
			"timeout": 30
		}
    }
}
```

### VSCode（Copilot）

VSCode 暂不支持直接连接 SSE，需通过 `mcp-remote` 桥接，配置在 [`.vscode/mcp.json`](.vscode/mcp.json)：

```jsonc
{
    "servers": {
        "minecraft_mod_assistant": {
            // 依赖nodejs环境
            "command": "mcp-remote",
            "args": [
                "http://localhost:18766/sse",
                "--transport",
                "sse-only"
            ]
        }
    }
}
```