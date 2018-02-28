$(document).ready(function(e){
  $("#utton_right").click(function(e){
    if(confirm('确认删除？')){
    setNumber();
  }
  });
  $("#button_left").click(function(e){
    saveDevice();
  });


});

function saveDevice(){
  var k = document.getElementById("de_name").value;
  var i = document.getElementById("de_kind").value;
  var po = document.getElementById("de_num").value;
  var de = document.getElementById("de_position").value;
  var re = document.getElementById("de_belong").value;
  var pe = document.getElementById("de_person").value;
  if(k == "" || i == "" || po == "" || de == "" || re == "" || pe == "" ){
    alert("数据不能为空！");
    return false;
    }else{
      $.ajax({
        url:"/goform/goahead_dev_add",
      data:{
          de_name:$("#de_name").val(),
          de_kind:$("#de_kind").val(),
          de_position:$("#de_position").val(),
          de_belong:$("#de_belong").val(),
          de_num:$("#de_num").val(),
          de_person:$("#de_person").val(),
        },
        type:"POST",
        dataType:"text",
        async: true,
        // contentType:"application/x-www-form-urlencoded; charset=UTF-8"
        success: function(data){    //    alert后台的返回值
          if(data == 0){
          alert("添加成功！")
         }else if(data == -1){
           alert("添加失败！");
         }
           },
      });
      return true;
    }
}
function setNumber(){
    $.ajax({
      cache: true,
      url:"/goform/goahead_dev_delete",
      data:$("#form_list").serialize(),
      type:"POST",
      dataType:"text",
      async: true,
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
