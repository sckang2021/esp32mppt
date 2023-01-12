# ESP32 MPPT
![输入图片说明](%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20221013212635.jpg)
![输入图片说明](%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20221013212929.jpg)


#### 介绍
基于esp32-wroom-32的MPPT同步整流充电系统

此项目基于https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/

PCB工程修改自Gggggg的立创版本 esp32 mppt - 嘉立创EDA开源硬件平台 https://oshwhub.com/gggggg/esp32-1kw-mppt_copy

本项目立创EDA工程文件 Schematic PCB files

HW5.0：[https://oshwhub.com/muyan2020/wei-yan-zheng-esp32-guang-fu-pv-mppt-ina226](https://oshwhub.com/muyan2020/wei-yan-zheng-esp32-guang-fu-pv-mppt-ina226)

HW6.01：[https://oshwhub.com/muyan2020/wei-yan-zheng-esp32-guang-fu-pv-mppt-ina226_copy_copy_copy_copy_copy](https://oshwhub.com/muyan2020/wei-yan-zheng-esp32-guang-fu-pv-mppt-ina226_copy_copy_copy_copy_copy)



 **【降压同步整流版本HW5.01】** 

HW5.01的程序使用ARDUINO_MPPT_FIRMWARE_V2.1

本项目修改为2颗INA226方案，输入和输出的电压电流都能监测到

 **高端检测**  电压不要超过36V，否则可能损坏ina226

 **低端检测**  已验证，耐压没问题，不过输入端电流会包含esp32等内部耗电，程序部分需要自行修改 2_Read_Sensors.ino ，将

`CSI_converted = ina1.readShuntCurrent();`

修改为

`CSI_converted = -ina1.readShuntCurrent();`




 **【升压/降压同步整流版本HW6.01】** 

HW6.01的程序使用 https://gitee.com/muyan3000/esp32-mppt/tree/master/esp32_buck_boost/bin/HW6

 **TFT屏幕：1.8寸TFT LCD SPI TFT彩屏st7735驱动128x160** 

在降压版本的基础上，升压需要通过更换输入输出，外部开关来进行模式切换

 **为了避免意外发生，请仔细检查接入及切换开关位置！！！** 

 **_使用时将主板上的升降压开关切换至降压模式，在对应的端子上接入输入电源，开机_** 

此版本提供bin直接烧录，如果是第一次烧写使用非OTA固件，之后升级可使用OTA固件写入0x10000位置或者直接web ota升级

1、按图中打钩项选择，然后先按住boot，再按RESET，然后再全部松掉，进入download模式（可以通过串口助手看到进入烧录模式的提示）

![输入图片说明](%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20220901155947.png)

flash_download_tool工具（下载地址：[https://www.espressif.com/zh-hans/support/download/other-tools](https://www.espressif.com/zh-hans/support/download/other-tools) ），选择esp32进行烧写

建议先烧录https://gitee.com/muyan3000/esp32-mppt/blob/master/esp32_buck_boost/bin/i2cdetect.bin，I2C检测程序，检查INA226的地址是否正确
正常的TI INA226地址为0X40和0X41

2、烧写完成后，通过串口调试助手之类的串口工具，对输入输出电压进行查看校准

 **查看校准参数**  CHKVD


 **校准输入电压**  示例：INVD19.22

用万用表测量实际输入电压，例如为19.22

在串口发送界面输入：INVD19.22


 **校准输出电压**  示例：OUTVD16.22

用万用表测量实际输出电压，例如为16.22

在串口发送界面输入：OUTVD16.22


 **输入采样电阻设置**  示例 RNFA0.002

在串口发送界面输入：RNFA0.002

 **输出采样电阻设置**  示例 RNFB0.002

在串口发送界面输入：RNFB0.002


3、配网及blinker设置

第一次配置，通过设置界面，找到配网菜单，点击配网，会自动开启"ESP32_MPPT"的热点

使用手机wifi或电脑的wifi进行连接

手机端会自动跳转访问设置页，电脑端找一下网关地址，浏览器访问网关地址即可进行设置

如果已经配置过网络，则通过esp32的客户端地址访问

![输入图片说明](QQ%E5%9B%BE%E7%89%8720220909105704.png)


4、脱离串口的OTA固件升级，使用OTA对应的bin升级会保留设置数据，（请仔细检查固件对应的硬件版本进行升级，如果升级失败只能通过串口重新烧录）

OTA必须配网完成才能使用，可以通过浏览器访问 [http://ESP32_MPPT.local](http://) 或访问对应的客户端IP

OTA的账号：admin，密码：ESPmppt



![输入图片说明](QQ%E5%9B%BE%E7%89%8720220909105714.png)


5、深度休眠时间设置，默认为19点至5点间esp32进入超低功耗的深度休眠 20221013



然后就可以愉快地使用了

界面说明：
充电模式/电源模式的外框绿色表示开启了mppt，红色为cccv
buck/boost的外框绿色表示 pwm使能开启，红色为关闭
输入信息的外框绿色表示输入防逆流关闭，红色表示防逆流开启
wifi图标绿色表示wifi已连接，wifi图标下方为转换效率，右侧为电量百分比
Fan图标紫色表示风扇未开启，开启后为绿色数字显示风扇转速比，风扇状态右侧为温度显示
右下角为电压设置和电流设置，长按开启，开启后单击切换位数，双击切换电压或电流
左下角绿色ok为正常运行，红色为错误状态信息
最底下为当前开机输出电量

界面2
累计电量，开机时间，当前时间



![输入图片说明](%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20220901143819.jpg)
![输入图片说明](%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20220901115046.jpg)


#### 软件架构
基于ESP32的MPPT充电系统

远程采集使用blinker，界面在程序目录中，复制到blinker的界面配置即可

![输入图片说明](blinker_ui.jpg)


#### 安装教程
修改ARDUINO_MPPT_FIRMWARE_V2.1.ino文件中的wifi配置信息及blinkerid

Liquid Crystal I2C LCD Library (By: Robojax) 这个库不能用

用LiquidCrystal_I2C By:Frank de Brabander的版本

INA226的库名是：INA226Lib

/*

    INA226 Bi-directional Current/Power Monitor. Simple Example.

    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/cyfrowy-czujnik-pradu-mocy-ina226.html

    GIT: https://github.com/jarzebski/Arduino-INA226

    Web: http://www.jarzebski.pl

    (c) 2014 by Korneliusz Jarzebski

*/

#### 使用说明
各版本looptime比较

INA226版本
![输入图片说明](%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20220726114642.png)

ads1015版本
![输入图片说明](%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20220726120507.png)

ads1115版本
![输入图片说明](%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20220726114412.png)


#### 更新说明

 **20220706** 

修改充电算法中predictivePWM() 算法 ，解决压差小于6V时，输出电压大于电池电压的问题

 **20220902** 

升/降压版本：

增加WEB配网，同时设置Blinker ID

OTA webupdate

校准输入电压 INVD19.22

校准输出电压 OUTVD16.22

查看校准参数 CHKVD

修正首次烧录数据错乱问题

 **20220904** 

new UI

 **20220924** 

采用多点采样快速查找最大功率点
修复wifi自动重连逻辑

 **20221013** 

修改 blinker ui，去除累计费用，更新为当天电量及累计电量

 **20221015** 

修改波特率为576000，本文档增加采样电阻设置

 **20221016** 

增加动态风扇开关，可以使用默认的温控开关风扇模式，也可以使用2线风扇温控转速控制

 **20221118** 

增加BNC状态下，电池保护激活，每30秒尝试激活一次

esp32_buck_boost_V3.5.10版本关闭多点检测

 **20221216** 

增加

wifi自动连接限定100次，之后每半小时重试

修正

boost算法调整，工作最低电压为4v

BNC状态下电池激活问题

休眠设置禁用无效问题

buck模式在某些条件下无法限流问题

 **20221219** 

新增

Web界面数据信息

通过浏览器访问 http://ESP32_MPPT.local:88 或访问对应的客户端IP http://客户端IP:88


修正

buck模式下限压限流问题及mppt等相关算法调整


 **20230109** 

HW：6.11

修改LDO 3.3v 为 DC-DC 3.3v

V6.11去除升降压手动开关，将IO17作为输出控制的IO,开启为低电平，关闭为高电平

V6.02使用时只需将升降压开关的3个引脚引出即可


FW：3.5.41 增加外部输出控制功能

设置页首页双击，可开关输出

电池输出控制基于设定的最低电压，电池电压大于最低电压，输出控制为1、2、3（常开），则深度休眠失效：

1、自动控制：无输入则自动开启输出

2、时间控制：以深度休眠时间作为开启时间

3、手动开关


#### 参与贡献

高冧，熊喵，龙卷风，小一科技


