$(document).ready(function(e) {
	$("#checkbox").click(function(e){
		timeLock();
	})
	$("#pres_conf").click(function(e){
    saveConf();
	})

	// $("#checkbox").focus();
	// $("#t_set").focus();
	// $("#timezone").focus();
	// $("#time_server").focus();
	function timeLock(){
		var t = null;
		t = setTimeout(time,1000);//开始执行
		 function time()
	 {
			clearTimeout(t);//清除定时器
			dt = new Date();
			var h=dt.getHours();
			var m=dt.getMinutes();
			var s=dt.getSeconds();
			document.getElementById("time_set1").value = h;
			document.getElementById("time_set2").value = m;
			document.getElementById("time_set3").value = s;
			t = setTimeout(time,1000); //设定定时器，循环执行
	 }
	}

  function saveConf(){
  		$.ajax({
  		url:"/goform/set_time",
			data:{
					checkbox:$("#checkbox").val(),
					time_set1:$("#time_set1").val(),
					time_set2:$("#time_set2").val(),
					time_set3:$("#time_set3").val(),
          			timezone:$("#timezone").val(),
          			tserver:$("#tserver").val()
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

});
  // }
