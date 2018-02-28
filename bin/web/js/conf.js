$(document).ready(function(e) {
	$("#pres_conf").click(function(e){
    selectOne();
	})
});
  function selectOne(){
  	var ct = document.getElementById("con_type").value;
  	var na = document.getElementById("net_add").value;

		var str=document.getElementsByName("box");
		var objarray=str.length;
		var chestr="";
		for (i=0;i<objarray;i++)
		{
			if(str[i].checked == true)
			{
			 chestr+=str[i].value+",";
			}
		}
		if(chestr == "")
		{
			alert("请先选择!");
		}
  	if(ct == "" || na == ""){
  		alert("请填写完整！");
  		return false;
  	}else{
  		$.ajax({
  			url:"/goform/set_relation",
			data:{
					con_type:$("#con_type").val(),
					net_add:$("#net_add").val(),
				},
  			type:"POST",
  			dataType:"text",
  			async: true,
				// contentType:"application/x-www-form-urlencoded; charset=UTF-8"
        success: function(data){    //    alert后台的返回值
               alert("访问成功");
           },
        error: function(){
               alert('访问失败');
           }
  		});
  		return false;
  	}
  }

	function checkbox()
	{
	var str=document.getElementsByName("box");
	var objarray=str.length;
	var chestr="";
	for (i=0;i<objarray;i++)
	{
	  if(str[i].checked == true)
	  {
	   chestr+=str[i].value+",";
	  }
	}
	if(chestr == "")
	{
	  alert("请先选择!");
	}

	}


	$(function(){
	//为指定name的radio设置选中值
	var param1=2;
	var param2=1;
	$(":radio[name=radio1][value="+param1+"]").attr("checked","true");//指定value的选项为选中项
	$(":radio[name=radio2][value="+param2+"]").attr("checked","true");
	//取值
	$(".default").click(function(){
	var radio1Value=$(":radio[name='radio1']:checked").val();
	var radio2Value=$(":radio[name='radio2']:checked").val();
	//测试
	alert("允许匿名登录："+radio1Value);
	alert("自动发现APP关联："+radio2Value);
	});
	});
