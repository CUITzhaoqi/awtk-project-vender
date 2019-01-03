AWTK-PROJECT-VENDER
^^^^^^^^^^^^^^^^^^^^^^

.. image:: http://gitlab.zhaoqi.info/awtk/awtk-project-vender/badges/master/pipeline.svg

.. image:: http://gitlab.zhaoqi.info/awtk/awtk-project-vender/badges/master/coverage.svg

AWTK-PROJECT-VENDER是AWTK的一个实例，主要用于演示如何快速将 `AWTK <https://github.com/zlgopen/awtk>`_ 应用于您的产品中。

如何使用
---------

首先克隆 awtk-project-vender

.. code-block:: bash

    git clone https://github.com/CUITzhaoqi/awtk-project-vender.git

然后更新子模块

.. code-block:: bash

    git submodule update --init --recursive -- "awtk"

**AWTK-PROJECT-VENDER** 依赖于二维码控件，所以请首先编译 `AWTK-QR <https://github.com/CUITzhaoqi/awtk-qr>`_ 。

将编译好的lib( **awtk_qr.lib** **qrencode.lib** )放入 **vender\\lib** 下，然后拷贝 `AWTK-QR <https://github.com/CUITzhaoqi/awtk-qr>`_ 下 **src\\qr**, **src\\lib\\qrencode** 的头文件到 **AWTK-PROJECT-VENDER** 下 **vender\\src** 中。


目录树
>>>>>>>>

- awtk\\………………(awtk工程)
- vender\\………………(自动售货机目录)
	
    + scripts\\……………………(awtk的工具脚本)

    - src\\……………………(自动售货机相关源码)
	
	- .SConstruct………………(scons 脚本)
	
- docs\\………………(相关文档)

Windows
>>>>>>>>>

(请先安装python2.7，scons和Visual Studio C++(版本>=2017)

编译运行(在命令行模式下，进入awtk-project-vender所在的目录，并运行下列命令)：

.. code-block:: bash

    scons -j8

更新资源(详细见 `更新资源 <https://github.com/zlgopen/awtk/tree/master/scripts>`_:)

.. code-block:: bash

    python ./scripts/update_res.py all

运行

.. code-block:: bash

    bin\vender.exe

文档
--------


问题
--------

报告BUG
>>>>>>>>>

请访问 `issue <https://github.com/CUITzhaoqi/awtk-project-vender/issues>`_:

许可证
---------

`awtk <https://github.com/zlgopen/awtk>`_: LGPLV2.1

`awtk-qr <https://github.com/CUITzhaoqi/awtk-qr>`_: LGPLV2.1


