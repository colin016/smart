<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>系统管理-用户管理界面</title>
    <link href="css/user_management.css" rel="stylesheet">
    <link href="css/index.css" rel="stylesheet">
    <script type="text/javascript" src="js/jquery-3.1.1.min.js"></script>
    <script type="text/javascript" src="js/user_management.js"></script>

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
          //单个按钮实现全选和取消：定义一个全局变量
      var checked = false;

      $("input#allChecked").click(function() {
           if (checked==false) {
              $("input.box").each(function() { // 遍历选中的checkbox
              $('table#table_id_example input[type=checkbox]').attr('checked', true);

            });
              checked=true;
           }else if (checked==true) {
            $("table#table_id_example input[type=checkbox]").each(function() {
            $(this).attr("checked", false);
            });
            checked=false;
           }
        });
    $("input#allChecked").click(function() {
           if (checked==false) {
              $("input.box").each(function() { // 遍历选中的checkbox
              $('table#dev_list input[type=checkbox]').attr('checked', true);
            });
              checked=true;
           }else if (checked==true) {
            $("table#dev_list input[type=checkbox]").each(function() {
            $(this).attr("checked", false);
            });
            checked=false;
           }
        });

      $("div.hidden_content").hide();
      $("div.cover").hide();
      $("input.button_left").click(function(){
        $("div.hidden_content").show();
        $("div.cover").show();
        $("div.cover").css('Opacity','0.5');
      });
      $("input#save").click(function(){
        $("div.hidden_content").hide();
        $("div.cover").hide();
      });
      $("#delete").click(function(e){
        $("input.op_content").val("");
      });
      $("img").click(function(){
        $("div.hidden_content").hide();
      });
    });

    //   $("div.cover").css("display","none");
    //   $("div.hidden_content").css("display","none");
    //   $("div.add_user i.add").click(function(){
    //     $("div.cover").css({"display":"block","opacity":"0.6"});
    //     $("div.hidden_content").css("display","block");
    //   })

    //   $("input.add_bu").click(function(){
    //     $("div.cover").css("display","none");
    //     $("div.hidden_content").css("display","none");
    //   })
    // });
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
    <div id="main_content">
      <p id="common_title">用户管理</p>
      <div class="main_content">       
        <div class="message_list">
          <!-- <div class="list_content"> -->
              <div class="tableScrollBox">
                  <table id="table_id_example" class="display">
                    <div class="title">
                      <tr style="background: #eee;">
                          <th class="ch_box">                             
                                  <input name="choiceAll" id="allChecked" class="allChecked" type="button" value="全选" />                            
                          </th>
                          <th>用户ID</th>
                          <th>用户类型</th>
                      </tr> 
                    </div>                      
                  </table>

                <div class="table_ma_con">
                  <table class="tableStyle2" id="dev_list_bo">
                    <%user_manage();%>
                  </table>
                </div>                
              </div>  
            <!-- </div>   -->
            <div id="two_button">
                  <input type="button" value="添加" class="button_left"/>
                  <input type="button" value="删除" class="button_right"/>
               </div>      
        </div>
      </div>
    </div>
     <!-- <div class="add_user"> -->
    <div class="hidden_content"> 
     <img src="img/cancle.png">  
    <!-- <div class="text_filine">
          <span class="icon">ID</span>
          <input type="text" value="" id="de_name" class="de_txt" name="de_name"/>
        </div>  --> 
        <div class="text_filine">
          <span class="add_op">用户ID</span>
            <input type="text" class="op_content" id="identity" name="identity"/>
        </div> 

        <div class="text_filine">
          <span class="add_op">用户名称</span>
            <input type="text" class="op_content" id="username" name="username"/>
        </div> 
            

            <!-- <input type="button" value="添加" class="add_sig" name="add_sig" id="add_sig"> -->
            <div class="su_bu">
              <input type="button" value="确认" onclick="" id="save" class="add_de"/>
              <input type="button" value="清空" onclick="" id="delete" class="add_de"/>
            </div>
      </div>
    <!-- <div class="cover"></div>  -->

  
  </body>
</html>
