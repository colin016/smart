$(document).ready(function(e) {
	$("#content_submit").click(function(e){
		// test();
		reExit();
	})
});

function test() {
	var regex = /^[^_][A-Za-z]*[a-z0-9_]*$/ ;

}
  function reExit(){
  	var nt = document.getElementById("name_txt").value;
  	var pt = document.getElementById("password_txt").value;
    var y = document.getElementById("yes").value;
		var regex = /^[^_][A-Za-z]*[a-z0-9_]*$/ ;
		var ret1 = regex.test(nt);
		var ret2 = regex.test(pt);
  	if(nt == "" || pt == ""){
  		alert("不能输入为空！");
  		return false;
		}else if(ret1 == false || ret2 == false){
				alert("输入错误！（只能输入数字和字母）");
    }else if(nt.length < 6 || pt.length < 6){
      alert("输入错误！（密码或用户名不得小于六位）");
  	}else{

  		$.ajax({
  			url:"/goform/register_exit",
			data:{
					name_txt:$("#name_txt").val(),
          yes:$("#yes").val(),
					password_txt:$("#password_txt").val()
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
  		return false;
  	}
  }
