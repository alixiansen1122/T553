启用systemview调试。 注意：当前仅适配liteos版本。源代码需要用户获取，SDK仅发布litesos适配部分，建议使用v3.42版本。
1)在config.py对应target增加systemview组件。
2)在kernel/liteos/liteos_v207.1.0/Huawei_LiteOS/tools/build/config/brandy.config将LOSCFG_SYSTEM_VIEW配置为y
3)将systemview源代码放置application/wearable/SystemView/Src下，其中Sample中sdk已发布了LiteOS适配。
4)SystemView/Src/Sample/LiteOS/Tooladapt下SYSVIEW_LiteOS.txt放置在PC端SYSTEMVIEW工具的Description文件夹下。


在线模式。
1.在SEGGER_SYSVIEW_ConfDefaults.h中将SEGGER_SYSVIEW_POST_MORTEM_MODE配置为0
2.在SEGGER_SYSVIEW_ConfDefaults.h中将SEGGER_SYSVIEW_RTT_BUFFER_SIZE修改为(16 * 1024)
3.通过application.lst中获取_SEGGER_RTT 变量地址。
4.通过工具的recorder configuration配置RTT的Detection地址，然后启动采集。

