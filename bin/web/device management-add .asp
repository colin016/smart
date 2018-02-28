<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>设备管理-媒介列表界面</title>
    <link href="css/media_list.css" rel="stylesheet">
    <link href="css/index.css" rel="stylesheet">
    <script type="text/javascript" src="js/jquery-3.1.1.min.js"></script>
    <script type="text/javascript" src="js/media_list.js"></script>
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
      $(".su").click(function() {
        for(var i=0; i<$("input[name='choice']").length;i++){
          if($($("input[name='choice']").eq(i)).is(":checked")){
            $($("input[name='choice']").eq(i)).parent().parent().remove();
          }
        }
      });

      //单个按钮实现全选和取消：定义一个全局变量
      var checked = false;

      $("input#allChecked").click(function() {
           if (checked==false) {
              $("input.box").each(function() { // 遍历选中的checkbox
              $('table#dev_list_bo input[type=checkbox]').attr('checked', true);

            });
              checked=true;
           }else if (checked==true) {
            $("table#dev_list_bo input[type=checkbox]").each(function() {
            $(this).attr("checked", false);
            });
            checked=false;
           }
        });


      // var checked=false;
      // $("input#allChecked").click(function(){
      //   if(checked==false){
      //     $("input.box").each(function() { // 遍历选中的checkbox
      //     $('table#dev_list_bo input[type=checkbox]').attr('checked', true);
      //   });
      //     checked=true;
      //   }else if (checked==true) {
      //     $("input.box").each(function() { // 遍历选中的checkbox
      //     $('table#dev_list_bo input[type=checkbox]').attr('checked', false);
      //   });
      //     checked=true;
      //   }
      // })
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
        <li><a href="#"></a></li>
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
        <li><a href="device management-add.html">媒介列表<i></i></a></li>
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
<form method="post" id="form_list">
  <div id="main_content">
        <h1 id="title">媒介列表</h1>
        <div class="main_content">
          <div class="tableScrollBox" id="d1">
          <table class="tableStyle1" id="dev_list">
              <tr style="background: #eee;">
                <th class="ch_box">
                  <div>
                    <input name="choiceAll" id="allChecked" class="allChecked" type="button" value="全选" />
                  </div>
                </th>
                <th>媒介名称</th>
                <th>媒介类型</th>
                <th>设备编号</th>
                <th>媒介状态</th>
                <th>媒介连接设备数量</th>
              </tr>
          </table>
          <div class="table_ma_con">
            <table class="tableStyle2" id="dev_list_bo">
  
              <%media_list();%>
            </table>
          </div>
          <div id="two_button">
            <input type="button" value="添加" class="button_left"/>
                  <input type="button" value="删除" class="button_right"/>
          </div>
        </div>
  </div>
</form>
</body>
</html>