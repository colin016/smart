$(document).ready(function(e){
  $("#interval").click(function(e){
    terrSyn();
  });
});

function terrSyn(){
    $.ajax({
      url:"/goform/terrace_syn",
      data:{
        t_interval:$("#t_interval").val(),
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
