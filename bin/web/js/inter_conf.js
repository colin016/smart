$(document).ready(function(e) {
	$("#pres_conf").click(function(e){
    saveConf();
	})
});
  function saveConf(){
				//	window.open("./system_information.html","_blank");
				//	return false;
  	// var ct = document.getElementById("con_type").value;
  	var na = document.getElementById("net_add").value;
    var sm = document.getElementById("subnet_mask").value;
    var dg = document.getElementById("default_gateway").value;
    var md = document.getElementById("main_dns").value;
    var od = document.getElementById("other_dns").value;
  	if(na == "" || sm == ""|| dg == "" || md == "" || od == "" ){
  		alert("数据不能为空！");
  		return false;
  	}else{
  		$.ajax({
  			url:"/goform/set_network",
			data:{
					connection_type:$("#connection_type").val(),
					net_add:$("#net_add").val(),
          subnet_mask:$("#subnet_mask").val(),
          default_gateway:$("#default_gateway").val(),
          main_dns:$("#main_dns").val(),
          other_dns:$("#other_dns").val()
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
