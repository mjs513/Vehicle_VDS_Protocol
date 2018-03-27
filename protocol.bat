@echo off

:: 判断是否定义copy_config，若已定义，则跳过手动输入部分
if defined copy_config goto define_original_file

:: 若未定义copy_config，则通过手动输入判断
echo **********提示***********
echo 输入“1” 复制Debug目录下dll文件
echo 输入“2” 复制Release目录下dll文件
echo 输入“3” 复制SO文件
echo *************************
set /p Copycopy_config=请选择输入参数:
if %Copycopy_config%==1 (
	SET copy_config=Debug
)
if %Copycopy_config%==2 (
	SET copy_config=Release
)
if %Copycopy_config%==3 (
	SET copy_config=Android
)

:define_original_file
if %copy_config%==Android (
	set cmd_copy_original_file=.\libs\armeabi\*.so
)
if %copy_config%==Release (
	set cmd_copy_original_file=.\release\*.dll
)
if %copy_config%==Debug (
	set cmd_copy_original_file=.\debug\*.dll
)

::*******复制文件************************************************************************************************************
::*******************************************************************************************************************************
:: 应采用相对路径，避免不同人拷贝到不同目录下时出现问题；
set cmd_copy_direction_path=..\..\Windows版\Diagnostics\DiagnosticSysRes\系统文件名\ActiveFolder

:: 命令行运行或双击运行此批处理文件的路径比VC生成后事件调用的路径高一级，故VC调用时的路径再升一级；
set VC_copy_original_file=..\%cmd_copy_original_file%
set VC_copy_direction_path=..\%cmd_copy_direction_path%

:: 通过判断cmd_copy_direction_path文件是否存在，确定是直接运行此批处理文件或VC生成后事件调用
if exist "%cmd_copy_direction_path%" (
	:: 存在cmd_copy_direction_path路径，说明是命令行运行，再判断源文件是否存在
	if exist "%cmd_copy_original_file%" (
		xcopy /e/s/y "%cmd_copy_original_file%" "%cmd_copy_direction_path%"
	) else (
		echo 源文件不存在: "%cmd_copy_original_file%"
		start /wait "提示"  cmd /c "mode con  cols=50 lines=10 &echo.源文件目录不存在：（"%cmd_copy_original_file%"）,按任意键继续...&pause>nul"
		pause
	)
) else (
	:: cmd_copy_direction_path不存在，可能是VC调用，导致路径不一致，也可能是真的不存在
	if exist "%VC_copy_direction_path%" ( 
		xcopy /e/s/y "%VC_copy_original_file%" "%VC_copy_direction_path%"
	) else (
		:: 若不存在，应手动创建，避免仅有动态库，而无其他相关文件；
		echo 目标文件夹不存在: "%VC_copy_direction_path%"
		start /wait "提示"  cmd /c "mode con  cols=50 lines=10 &echo.目标文件目录不存在：（"%VC_copy_direction_path%"）,按任意键继续...&pause>nul"
		pause
	)
)
pause




