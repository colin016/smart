$(document).ready(function(e){
  $("#save_device").click(function(e){
    saveDevice();
  });

});

function saveDevice(){
  var un = document.getElementById("username").value;
  var on = document.getElementById("othername").value;
  var j = document.getElementById("jurisdiction").value;
  var regex = /^[^_][A-Za-z]*[a-z0-9_]*$/ ;
  var ret1 = regex.test(un);
  var ret2 = regex.test(on);
  if(un == "" || on == "" || j == ""){
    alert("数据不能为空！");
    return false;
    }else if(ret1 == false || ret2 == false){
      alert("输入错误！（只能输入数字和字母）");
    }else if(un.length < 6 || on.length < 6){
      alert("输入错误！（别名和用户名不得小于六位）");
    }else{
      $.ajax({
        url:"/goform/contact_person",
      data:{
          username:$("#username").val(),
          othername:$("#othername").val(),
          jurisdiction:$("#jurisdiction").val()
        },
        type:"POST",
        dataType:"text",
        async: true,
        // contentType:"application/x-www-form-urlencoded; charset=UTF-8"
        success: function(data){    //    alert后台的返回值
					if(data == 0){
				 	alert("添加成功！")
				 }else if(data == -1){
					 alert("添加失败！");
				 }
           },
      });
      return true;
    }
}
