$(document).ready(function(e) {
	$("#sign_in").click(function(e){
    userSet();
	});
  $("#write_off").click(function(e){
    userSet();
  })
});
  function userSet(){
				//	window.open("./system_information.html","_blank");
				//	return false;
  	var un = document.getElementById("us_name").value;
	var p = document.getElementById("pass").value;
    var sn = document.getElementById("serial_number").value;
    var l = document.getElementById("longitude").value;
    var la = document.getElementById("latitude").value;
    var mn = document.getElementById("max_num").value;
    var regex = /^[^_][A-Za-z]*[a-z0-9_]*$/ ;
		// var inputValue1 = document.getElementById("password").value;
    // var inputValue2 = document.getElementById("username").value;
		var ret1 = regex.test(un);
    var ret2 = regex.test(p);
    var ret3 = regex.test(sn);
  	if(un == "" || p == "" || sn == ""|| l == "" || la == "" || mn == "" ){
  		alert("数据不能为空！");
  		return false;
    }else if(ret1 == false || ret2 == false || ret3 == false){
      alert("输入错误！（用户名和密码和序列号只能输入数字和字母）");
    }else if(un.length < 6 || p.length < 6){
      alert("输入错误！（密码或用户名不得小于六位）");
  	}else{
  		$.ajax({
  			url:"/goform/set_user",
			data:{
					us_name:$("#us_name").val(),
					pass:$("#pass").val(),
          longitude:$("#longitude").val(),
          latitude:$("#latitude").val(),
          max_num:$("#max_num").val(),
          serial_number:$("#serial_number").val()
				},
  			type:"POST",
  			dataType:"text",
  			async: true,
				// contentType:"application/x-www-form-urlencoded; charset=UTF-8"
        success: function(data){    //    alert后台的返回值
               if(data == 0){
				 	alert("访问成功！")
				 }else if(data == -1){
					 alert("访问失败！");
				 }
           },
  		});
  		return true;
  	}
  }
