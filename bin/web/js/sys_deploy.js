$(document).ready(function(e){
  $("#de_sub").click(function(e){
    sys_de();
  });
})
function sys_de(){
  var dn = document.getElementById('device_name').value;
  var cw = document.getElementById("contact_way").value;
  var po = document.getElementById("position").value;

  if(dn ==''|| cw ==''||po==''){
    alert("数据不为空！");
    return false;
  }else{
    $.ajax({
      url:"/goform/sys_deploy",
      data:{
        device_name:$("#device_name").val(),
        contact_way:$("#contact_way").val(),
        position:$("#position").val()
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
