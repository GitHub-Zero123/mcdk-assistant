# OriginGUIName

class in mod.common.minecraftEnum

- 描述

  获取原生UI名字

```python
class OriginGUIName(object):
	MoveUpBtn = "binding.area.move_up" 		# 上移键
	MoveDownBtn = "binding.area.move_down"  # 下移键
	MoveLeftBtn = "binding.area.move_left"  # 左移键
	MoveRightBtn = "binding.area.move_right" # 右移键
	SneakBtn = "binding.area.sneak"	# 潜行键
	NewSneakBtn = "binding.area.sneak_new_controls" # 新触控（摇杆模式）潜行键/空中&水底下潜键
	JumpBtn = "binding.area.jump"	# 跳跃键
	NewJumpBtn = "binding.area.jump_new_controls" # 新触控（摇杆模式）跳跃键/空中&水底上浮键
	SprintBtn = "binding.area.sprint" # 冲刺按钮（切换跑/走）
	AscendBtn = "binding.area.ascend" # 右侧双击跳跃键飞行后操作按键： 上移
	DescendBtn = "binding.area.descend" # 右侧双击跳跃键飞行后操作按键： 下移
	PauseBtn = "binding.area.pause" # 暂停键
	ChatBtn = "binding.area.chat"	# 聊天按钮
	MenuBtn = "binding.area.fold_menu"	# 菜单按钮(截图分享)
	ReportBtn = "binding.area.report_cheat" # 举报按钮（已废弃）
	CameraViewBtn = "binding.area.camera_view" # 摄像机视角按钮
	DestroyOrAttackBtn = "binding.area.destroy_or_attack" # 破坏/攻击按钮
	BuildOrInteractBtn = "binding.area.build_or_interact" # 建造/交互按钮
	MoveStickBtn = "binding.area.default_move_stick_area" # 新触控摇杆按钮
	WalkState = "binding.area.walkstate"  # 方向键模式下强制疾跑按钮
	MobEffects = "binding.area.mobeffects"  # buff状态
	Emote = "binding.area.emote"  # 表情按钮

	# 以下枚举将会于后续版本加入，请开发者直接使用字符串
	"binding.area.turn_interact" # 转向交互
	"binding.area.dpad_no_turn_interact" # 方向键无转向交互
	"binding.area.disable_jump" # 禁用的跳跃
	"binding.area.disable_sneak" # 禁用的潜行
	"binding.area.gui_passthrough" # GUI穿透区域
	"binding.area.changing_flight_height" # 改变飞行高度
	"binding.area.move_up_invisible" # 隐藏的向上移动键
	"binding.area.middle_right" # 十字键模式上升下降面板区域
	"binding.area.code_builder" # 教育版编程按钮
	"binding.area.move_up_left" # 向左上方移动（十字键模式）
	"binding.area.move_up_right" # 向右上方移动（十字键模式）
	"binding.area.player_effects" # 玩家效果
	"binding.area.paddle_right" # 右侧划船按钮
	"binding.area.paddle_right_border" # 右侧划船按钮边界
	"binding.area.paddle_left" # 左侧划船按钮
	"binding.area.paddle_left_border" # 左侧划船按钮边界
	"binding.area.toast"# 提示区域
	"binding.area.keyjoy" # 按键/摇杆
	"binding.area.sneak_jk" # 潜行（按键/摇杆）
	"binding.area.store" # 商店按钮
	"binding.area.vstate0" # 视图状态0
	"binding.area.voice_trans" # 语音转文字按钮（若”我的好友“已启用，则会消失）钮

```
