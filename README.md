# ESP32 MPPT

#### 介绍
基于ESP32的MPPT同步整流充电系统

此项目基于https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/

PCB工程修改自Gggggg的立创版本 esp32 mppt - 嘉立创EDA开源硬件平台 https://oshwhub.com/gggggg/esp32-1kw-mppt_copy

本项目对应立创EDA工程文件：https://oshwhub.com/muyan2020/wei-yan-zheng-esp32-guang-fu-pv-mppt-ina226


 **【降压同步整流版本】** 

本项目修改为2颗INA226方案，输入和输出的电压电流都能监测到

 **高端检测**  电压不要超过36V，否则可能损坏ina226

 **低端检测**  已验证，耐压没问题，不过输入端电流会包含esp32等用电设备，程序部分需要自行修改 2_Read_Sensors.ino ，将

`CSI_converted = ina1.readShuntCurrent();`

修改为

`CSI_converted = -ina1.readShuntCurrent();`



 **【升压/降压同步整流版本】** 

此版本提供bin直接烧录，省得各种配置劝退

按途中打钩项选择，然后按boot+en进入download模式

![输入图片说明](%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20220901155947.png)

flash_download_tool工具（下载地址：[https://www.espressif.com/zh-hans/support/download/other-tools](https://www.espressif.com/zh-hans/support/download/other-tools) ），选择esp32进行烧写

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
20220706

修改充电算法中predictivePWM() 算法 ，解决压差小于6V时，输出电压大于电池电压的问题

20220902

升/降压版本：

增加WEB配网，同时设置Blinker ID

OTA webupdate

校准输入电压 INVD19.22

校准输出电压 OUTVD16.22

查看校准参数 CHKVD


#### 参与贡献
高冧，熊喵，龙卷风，小一科技


