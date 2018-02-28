$(document).ready(function(e){
  $("#save").click(function(e){
    terrInfo();
  });
});

function terrInfo(){
  var an = document.getElementById('acc_num').value;
  var op = document.getElementById("old_password").value;
  var np = document.getElementById("new_password").value;
  var y = document.getElementById("yes").value;

  if(an ==''|| op ==''||np==''){
    alert("数据不为空！");
    return false;
  }else{
    $.ajax({
      url:"/goform/terrace_info",
      data:{
        acc_num:$("#acc_num").val(),
        old_password:$("#old_password").val(),
        new_password:$("#new_password").val(),
        yes:$("#yes").val()
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
  }
  return true;
}
