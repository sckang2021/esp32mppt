# ESP32 MPPT

#### 介绍
基于ESP32的MPPT充电系统
此项目基于https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/
PCB工程修改自Gggggg的立创版本 esp32 mppt - 嘉立创EDA开源硬件平台 https://oshwhub.com/gggggg/esp32-1kw-mppt_copy

本项目修改为2颗INA226方案，输入和输出的电压电流都能监测到

高压端检测版本 电压不要超过36V，否则可能损坏ina226

低压端检测版本未画完


#### 软件架构
远程采集使用blinker，界面在程序目录中，复制到blinker的界面配置即可

INA226的库名是：INA226Lib

/*

    INA226 Bi-directional Current/Power Monitor. Simple Example.

    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/cyfrowy-czujnik-pradu-mocy-ina226.html

    GIT: https://github.com/jarzebski/Arduino-INA226

    Web: http://www.jarzebski.pl

    (c) 2014 by Korneliusz Jarzebski

*/


#### 安装教程
修改ARDUINO_MPPT_FIRMWARE_V2.1.ino文件中的wifi配置信息及blinkerid


#### 使用说明



#### 参与贡献



