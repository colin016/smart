$(document).ready(function(e){
  $(".button_left").click(function(e){
    if(confirm('该信息已添加')){
      setNumber();
    }
  });
});

$(document).ready(function(e){
  $(".button_right").click(function(e){
    if(confirm('确定删除？')){
      setNumber();
    }

  });
});

  function setNumber(){
  		$.ajax({
  			cache: true,
  			url:"/goform/del_ssconfig",
  			data:$("#form_list").serialize(),
  			type:"POST",
  			dataType:"text",
  			async: false,
  			success: function(data){
  				if(data==1 ){
  					alert("删除成功！");
  					window.location.reload();
  				}else if(data == 2){
  					alert("请选择要删除的!");
  					window.location.reload();
  				}else{
  					alert("删除失败！");
  					window.location.reload();
  				}
  			}
  		});
};
