<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>系统管理-监控库管理界面</title>
    <link href="css/monitor_manager.css" rel="stylesheet">
    <link href="css/index.css" rel="stylesheet">
    <script type="text/javascript" src="js/jquery-3.1.1.min.js"></script>
    <!-- DataTables CSS -->
    <link rel="stylesheet" type="text/css" href="http://cdn.datatables.net/1.10.13/css/jquery.dataTables.css">

    <!-- jQuery -->
    <script type="text/javascript" charset="utf8" src="http://code.jquery.com/jquery-1.10.2.min.js"></script>

    <!-- DataTables -->
    <script type="text/javascript" charset="utf8" src="http://cdn.datatables.net/1.10.13/js/jquery.dataTables.js"></script>

    <!--或者本地-->
    <!-- DataTables CSS -->
    <link rel="stylesheet" type="text/css" href="DataTables-1.10.13/media/css/jquery.dataTables.css">
    <!-- jQuery -->
    <script type="text/javascript" charset="utf8" src="DataTables-1.10.13/media/js/jquery.js"></script>

    <!-- DataTables -->
    <script type="text/javascript" charset="utf8" src="DataTables-1.10.13/media/js/jquery.dataTables.js"></script>
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
      $('#table_id_example').dataTable( {
        "autoWidth": false,
        "info": "显示第 _START_ 至 _END_ 项结果，共 _TOTAL_ 项",
        "sInfoEmpty": "显示第 0 至 0 项结果，共 0 项",
        "lengthChange": false,
         "pageLength": 5,
         "language": {
          "emptyTable": "表中没有数据！",
          "search": "搜索:",
          "paginate": {
            "next": "下一页",
            "last": "最后",
            "previous": "上一页"

          }
        }
      } );
    });
    </script>

<!-- 头部和导航栏 -->
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
      <h1 id="title">监控库管理</h1>
      <div class="main_content">
        <div class="monitor_list">
          <div class="monitor_left">当前监控库信息</div>
          <div class="list_content">
            <table id="table_id_example" class="display">
              <thead>
                  <tr>
                      <th>序号</th>
                      <th>内容</th>
                  </tr>
              </thead>
              <!--<tbody>
                    <tr>
                        <td class="number">1</td>
                        <td>内容</td>
                    </tr>
                    <tr>
                      <td class="number">2</td>
                      <td>内容</td>
                    </tr>
                    <tr>
                        <td class="number">3</td>
                        <td>内容</td>
                    </tr>
                    <tr>
                        <td class="number">4</td>
                        <td>内容</td>
                    </tr>
                    <tr>
                        <td class="number">5</td>
                        <td>内容</td>
                    </tr>
                    <tr>
                        <td class="number">6</td>
                        <td>内容</td>
                    </tr>
                    <tr>
                        <td class="number">7</td>
                        <td>内容</td>
                    </tr>
                    <tr>
                        <td class="number">8</td>
                        <td>内容</td>
                    </tr>
                    <tr>
                        <td class="number">9</td>
                        <td>内容</td>
                    </tr>
                    <tr>
                        <td class="number">10</td>
                        <td>内容</td>
                    </tr>
					</tbody>-->
					<%monitor_sys();%>
            </table>
          </div>
        </div>
        <div class="other">
		<div  class="wout-table">
         <table cellpadding="0" cellspacing="0">
          <div class="first_line">
            <p class="name">库名称：</p>
            <span class="whrite"></span>
            <p class="name">版本：</p>
            <span class="whrite"></span>
          </div>
          <div class="second_line">
            <p class="name">安装日期：</p>
            <span class="whrite"></span>
            <p class="name">工作状态：</p>
            <span class="whrite"></span>
          </div>
		</table>
		</div>
          <div class="third_line">
            <p class="name">更新库：</p><input type="file" id="file_up"/>
          </div>
        </div>
      </div>
    </div>
  </body>
</html>
