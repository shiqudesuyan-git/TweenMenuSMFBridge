# Tween Menu → SKSE Menu Framework

在 Tween Menu Overhaul 中增加 **SKSE MOD SETTINGS** 入口。选中后，Tween Menu 关闭，接着打开 SKSE Menu Framework 的主菜单，可以进入已经在框架中注册的各个模组页面。

Classic 和 Alternative 两种 Tween Menu 布局都使用同一份补丁。原有 Open MCM 入口可以同时安装。

## 安装

1. 已安装适配游戏版本的 SKSE64、Address Library、SkyUI、Tween Menu Overhaul，以及 SKSE Menu Framework。
2. 用 MO2 或 Vortex 安装 `TweenMenuSMFBridge-1.0.0-NG.zip`，启用模组。
3. 用 SKSE 启动游戏，读档，打开 Tween Menu。
4. Classic 布局默认在 Journal 所在分类找到 **SKSE MOD SETTINGS**；Alternative 布局在横向列表中找到这个选项。

安装包包含 `SKSE/Plugins/TweenMenuSMFBridge.dll` 和 `Interface/tweenoptions/TweenMenuSMFBridge.json`，以及说明、许可证和验证记录。

## 兼容范围

- 使用 CommonLibSSE-NG，同时启用 SE 和 AE 支持。面向 SE 1.5.97 和 AE 1.6 系列；这表示编译支持范围，尚未进行各版本的游戏内验证。
- 当前构建没有启用 VR，也未声明 Skyrim 1.7 系列兼容性。
- 需要 SKSE Menu Framework 导出公共接口 `GetMainWindow`。已经核对作者当前 3.x 源码和 SDK 中的这个接口。旧版本缺少接口时，点击入口会提示升级，并写入日志。
- 独立 DLL 和独立 JSON，无 ESP/ESL，无 Papyrus 脚本。不会占用插件排序位置，也不保存存档数据。
- 使用框架的公共窗口接口，保留框架自己的快捷键、布局以及窗口的暂停/输入设置。

## 改成中文名称或移动位置

编辑模组中的 `Interface/tweenoptions/TweenMenuSMFBridge.json`：

```json
{
  "papyrus_event": "TweenMenuSMFBridge_Open",
  "menuName": "SKSE 模组菜单",
  "icon": "scroll",
  "priority": 80,
  "category": "right"
}
```

使用 UTF-8 保存并重新启动游戏。中文名称依赖你当前 Skyrim 界面字体对中文的支持。`menuName` 是显示名称，`category` 是 Classic 分类，`priority` 是排序；`papyrus_event` 必须保留。

## 出现问题时

- **看不到入口**：检查 JSON 和 DLL 都已部署在同一个虚拟/实际 Data 目录内，确认游戏中 Tween Menu Overhaul 本身正常加载。JSON 配置由 Tween Menu Overhaul 在启动时读取。
- **点击后提示框架未加载**：检查 `SKSEMenuFramework.dll` 的版本、依赖和 SKSE 加载日志。
- **提示缺少 GetMainWindow**：升级 SKSE Menu Framework 到提供此公共接口的版本。
- **提示框架尚未初始化**：等读档完成后重试。

插件日志：`文档/My Games/Skyrim Special Edition/SKSE/TweenMenuSMFBridge.log`；GOG 等版本可能使用不同的游戏文档目录，以 SKSE 的日志目录为准。

## 验证情况

本地进行 Windows x64 Release 编译，以及使用模拟框架 DLL 的集成检查。检查覆盖公开 DLL 接口、缺失依赖、缺失导出、初始化未完成、Tween 关闭前不打开、两种事件先后顺序、重复请求合并、读档取消和菜单再次打开的情况。

**这些检查不能替代 Skyrim 游戏内验证。此安装包尚未在真实游戏中运行。** 详细构建结果见 `VALIDATION.md`。

卸载时在模组管理器中禁用/移除此模组即可。

## 源码

源码随独立源码压缩包提供。`src/Plugin.cpp` 接收 Tween Menu 的事件，`src/OpenController.h` 管理菜单切换顺序，`src/FrameworkClient.cpp` 调用 SKSE Menu Framework 公共接口。构建方法见英文 README。

