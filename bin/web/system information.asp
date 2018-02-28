<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>状态信息-系统信息界面</title>
    <link href="css/system_information.css" rel="stylesheet">
    <link href="css/index.css" rel="stylesheet">
    <script type="text/javascript" src="js/jquery-3.1.1.min.js"></script>
  </head>
  <body>
    <script type="text/javascript">
    $(document).ready(function(){
    	$(".menu_son li").click(function(){
        $(".menu_son li").children().find("i").css("background-image","url('img/tran.png')");
        changeNode($(this).children().find("i"));
      });
      function changeNode(mainNode){
        if(mainNode){
          if(mainNode.css("background-image").indexOf("img/tran.png")>=0){
            mainNode.css("background-image","url('img/tran1.png')");
          }else {
            mainNode.css("background-image","url('img/tran.png')");
          }
        }
      }
    })

    var t = null;
   t = setTimeout(time,1000);//开始执行
   function time()
   {
      clearTimeout(t);//清除定时器
      dt = new Date();
      var h=dt.getHours();
      var m=dt.getMinutes();
      var s=dt.getSeconds();
      document.getElementById("timeShow").innerHTML =  "当前时间："+h+"时"+m+"分"+s+"秒";
      t = setTimeout(time,1000); //设定定时器，循环执行
   }
    </script>

    <div class="header">
      <p class="logo">10518</p>
      <p class="header_name">监控管理系统</p>
      <a href="system information.html" class="link_home"><img src="img/home.png" class="icon"/></a>
      <a href="login.html" class="link_out"><img src="img/out.png" class="icon"/></a>
    </div>
    <div class="index_left">
      <ul>
        <li class="menu one">状态信息
          <ul class="menu_son">
            <li><a href="system information.html">系统信息<i></i></a></li>
            <li><a href="inter information.html">网络信息<i></i></a></li>
            <li><a href="equipment information.html">设备信息<i></i></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <img src="img/icon.jpg" class="dot">
          </ul>
        </li>
        <li class="menu two">基本配置
          <ul class="menu_son">
            <li><a href="basic configuration-inter configuration.html">网络配置<i></i></a></li>
            <li><a href="basic configuration-time configuration.html">时间配置<i></i></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <img src="img/icon.jpg" class="dot">
          </ul>
        </li>
        <li class="menu three">设备管理
          <ul class="menu_son">
            <li><a href="device management-list.html">设备列表<i></i></a></li>
            <li><a href="device management-add.html">添加设备<i></i></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <img src="img/icon.jpg" class="dot">
          </ul>
        </li>
        <li class="menu four">监控管理
          <ul class="menu_son">
            <li><a href="monitor_manager.html">监控库管理<i></i></a></li>
            <li><a href="supervision_task.html" class="watch">监控任务管理<i></i></a></li>
            <li><a href="alarm_messages.html" class="watch">报警消息管理<i></i></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <img src="img/icon.jpg" class="dot">
          </ul>
        </li>
        <li class="menu five">云台管理
          <ul class="menu_son">
            <li><a href="aerial function-logon.html">登录管理<i></i></a></li>
            <li><a href="terrace_synchronization.html">云台同步<i></i></a></li>
            <li><a href="user_set.html">用户设置<i></i></a></li>
            <li><a href="register_exit.html">登录登出<i></i></a></li>
            <li><a href="terrace_information.html">云台信息<i></i></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <img src="img/icon.jpg" class="dot">
          </ul>
        </li>
        <li class="menu six">联系人管理
          <ul class="menu_son">
            <li><a href="contact management-look.html">查看关联人<i></i></a></li>
            <li><a href="affiliated_person_add.html">添加关联人<i></i></a></li>
            <li><a href="set_relation.html" class="rela">关联设置<i></i></a></li>
            <li><a href="affiliated_person_check.html">审核关联人<i></i></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <li><a href="#"></a></li>
            <img src="img/icon.jpg" class="dot">
          </ul>
        </li>
        <li class="menu seven">系统管理
          <ul class="menu_son">
            <li><a href="view_log.html">查看日志<i></i></a></li>
            <li><a href="system_deploy.html">系统配置<i></i></a></li>
            <li><a href="user_management.html">用户管理<i></i></a></li>
            <li><a href="uploading_plugins.html">上传插件<i></i></a></li>
            <li><a href="backup_restore.html">备份恢复<i></i></a></li>
            <li><a href="software update.html">软件更新<i></i></a></li>
            <li><a href="local_area_network.html" class="web">局域网发现管理<i></i></a></li>
            <li><a href="reboot_device.html">设备重启<i></i></a></li>
            <img src="img/icon.jpg" class="dot">
          </ul>
        </li>
      </ul>
    </div>



<!-- 主要内容 -->
    <div id="main_content">
      <h1 id="title">系统信息</h1>
      <div class="main_content">
        <div class="input">
          <th class="in-title" colspan="2"> <label id="timeShow"></lable></th>
        </div>
        <div class="wout-table">
          <table class=" table table-bordered confTable" cellpadding="0" cellspacing="0">
            <!--<tr>
            <td>软件版本：testV1.0</td>
            <td>运行时间：0天0时18分28秒</td>
            </tr>
            <tr>
            <td>设备型号：smart</td>
            <td>设备位置：office503</td>
            </tr>
            <tr>
            <td>内存利用率：3.22%</td>
            <td>cpu利用率：3.22%</td>
            </tr>
            <tr>
            <td>FLASH容量：0.88458G</td>
            <td>FLASH剩余量：0.88458G</td>
            </tr>-->
			<%sys_info();%>
          </table>
        </div>
      </div>
    </div>
  </body>
</html>
