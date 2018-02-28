$(document).ready(function(e){
  $("#delete").click(function(e){
    if(confirm('确认删除？')){
      if($('input.box').is(':checked')) {
        setNumber();
      }else {
        alert("未选中选项。")
      }
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
