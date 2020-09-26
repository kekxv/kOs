# flash 读写方法

此代码参考自  [利用STM32的FLASH模拟 EEPROM（F103）系列](https://blog.csdn.net/weixin_38604759/article/details/79774390)

## 原文说明

借鉴(~~照抄~~) [利用STM32的FLASH模拟 EEPROM（F103）系列](https://blog.csdn.net/weixin_38604759/article/details/79774390)

STM32的FLASH是用来存储主程序的，ST公司为了节约成本，没有加入 EEPROM，但是许多场合下我们需要用EEPROM；不过FLASH的容量还是可观的，我们可以利用FLASH模拟EEPROM。

根据《STM32F10X闪存编程》中的介绍，FLASH除了保存用户代码的部分，其余部分我们是可以利用其作为数据存储使用的。stm32的FLASH分为主存储块和信息块。主存储块用于保存具体的程序代码和用户数据，信息块用于负责由stm32出厂是放置2KB的启动程序（Bootloader）并锁死，用户无法更改。选项字节存储芯片的配置信息及对主存储块的保护信息。STM32的FLASH主存储块按页组织，有的产品每页1KB，有的产品每页2KB。页面典型的用途就是用于按页擦除FLASH。从这点来看，页面有点像通用FLASH的扇区。

通常情况下程序也不会把FLASH写满；在没满的时候我们可以把最后一或两页用来模拟EEPROM；这样我们就可以不用在外部另外来加EEPROM了。下面是STM32F103中文手册关于FLASH的截图；由于我用的是STM32C8T6做的实验；手册中没有给STM32C8T6的FLASH的地址信息，容量是64K，可以算出第63页地址是0X800FC00-0X800FFFF。