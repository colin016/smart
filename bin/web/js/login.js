$(document).ready(function(e) {
 // 	document.getElementById("username").value =
	// document.getElementById("password").value =

	$("#login_su").click(function(e){
		// test();
		loginCon();
	})
	$("#password").focus();
});

	// function test() {
	// 	var regex = /^[^_][A-Za-z]*[a-z0-9_]*$/ ;
	// 	var inputValue = document.getElementById("password").value;
	// 	var ret = regex.test(inputValue);
	// 		if (ret == false) {
	// 			alert("输入错误！（只能输入数字和字母）");
	// 		}
	// }

  function loginCon(){
  	var uName = document.getElementById("username").value;
  	var uPw = document.getElementById("password").value;
		var regex = /^[^_][A-Za-z]*[a-z0-9_]*$/ ;
		var inputValue1 = document.getElementById("password").value;
    var inputValue2 = document.getElementById("username").value;
		var ret1 = regex.test(inputValue1);
    var ret2 = regex.test(inputValue2);
  	if(uName == "" || uName == 0){
  		alert("请输入用户名！");
  		return false;
  	}else if(uPw == "" || uPw == 0){
  		alert("请输入密码！");
  		return false;
		}else if(ret1 == false || ret2 == false){
			alert("输入错误！（只能输入数字和字母）");
    }else if(uName.length < 6 || uPw.length < 6){
      alert("输入错误！（密码或用户名不得小于六位）");
  	}else{

  		$.ajax({
  			url:"/goform/login_server",
			data:{
					username:$("#username").val(),
					password:$("#password").val()
				},
  			type:"POST",
  			dataType:"text",
  			async: true,
				// contentType:"application/x-www-form-urlencoded; charset=UTF-8"
  			success: function(data){

  				if(data == 0){
					  window.location.href="./system information.html";
  				}
				else if(data == -1){
  					alert("密码或帐号错误！")
  				}
				else if(data == -2){
					alert("该账号没有管理权限！")
				}else{
					alert("没有该账户！");
				}
  			}
  		});
  		return false;
  	}
  }
/*
 * 界面登录
 * 返回值：
 * 	0:验证通过
 * 	-1：密码或帐号错误！
 * 	-2：该帐号没有管理权限！
 * 	-3：没有该帐号
 */