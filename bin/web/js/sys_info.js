$(document).ready(function(e){
  $.ajax({
    url:"/goform/sys_info",,//请求地址
	data:{
		   versions:$("#versions").val(), //在html页面id=ulul的标签里显示html内容
           run_time:$("#run_time").val(),
           unit_type:$("#unit_type").val(),
           run_timeunit_loc:$("#run_timeunit_loc").val(),
           memory_utilization:$("#memory_utilization").val(),
           cpu_utilization:$("#cpu_utilization").val(),
           flash_capacity:$("#flash_capacity").val(),
           flash_surplus:$("#flash_surplus").val()
	},
    dataType : "text",//数据格式
    type : "post",//请求方式
	async : false,//是否异步请求
    success : function(data) {
      //data是服务器返回数据
      alert("访问成功！");
	  /*
      alert(data);
      for(var i=0;i<data.length;i++){    //遍历data数组
             var ls = data[i];
             html +="<a href='second page text.html?newsid="+ls.news_id+"'class='infNews_wrod_a'>"+ls.news_name+"</a>";
         }
      var html = "";
           $("#versions").html(html); //在html页面id=ulul的标签里显示html内容
           $("#run_time").html(html);
           $("#unit_type").html(html);
           $("#unit_loc").html(html);
           $("#memory_utilization").html(html);
           $("#cpu_utilization").html(html);
           $("#flash_capacity").html(html);
           $("#flash_surplus").html(html);
    },
	
	*/
    error:function(){
      alert("访问失败！");
    }
  });
  return true;

})


function get_info()
{
	var result = "<%sys_info();%>";  //获取信息
	alert(result);
}


