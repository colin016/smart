$(document).ready(function(){
  $(".menu_son li").click(function(){
    $(".menu_son li").children().find("i").css("background-image","url('img/tran.png')");
    changeNode($(this).children().find("i"));
  });
  function changeNode(mainNode){
    if(mainNode){
      if(mainNode.css("background-image").indexOf("img/tran.png")>=0){
        mainNode.css("background-image","url('img/tran1.png')");
      }else {
        mainNode.css("background-image","url('img/tran.png')");
      }
    }
  }
  $('#table_id_example').dataTable( {
    "autoWidth": false,
    "info": "显示第 _START_ 至 _END_ 项结果，共 _TOTAL_ 项",
    "sInfoEmpty": "显示第 0 至 0 项结果，共 0 项",
    "lengthChange": false,
     "pageLength": 5,
     "language": {
      "emptyTable": "表中没有数据！",
      "search": "搜索:",
      "paginate": {
        "next": "下一页",
        "last": "最后",
        "previous": "上一页"

      }
    }
  } );

  $(".su").click(function() {
    for(var i=0; i<$("input[name='choice']").length;i++){
      if($($("input[name='choice']").eq(i)).is(":checked")){
        $($("input[name='choice']").eq(i)).parent().parent().remove();
      }
    }
  });

  $('input#deploy').click(function(){
    $('div.hidden1').show();
  });

  $('input#bu1').click(function(){
    $('div.hidden1').hide();
  })

  $('input#bu2').click(function(){
    $('div.hidden1').hide();
  })

  // function ck(){

    $('div.hidden1').hide();
    $('#third_row').hide();
    // $('#third_row').hide();
    // function gradeChange(){
    //   // alert("aaa");
    //   if(document.getElementById("control_kind").value == "0"){
    //     $('#third_row').show();
    //     alert("aaa");
    //   }else {
    //     alert("ccc")
    //   }
    // }
    // var ck = document.getElementById('control_kind').value;
    // if(document.getElementById("control_kind").value == "0"){
    //   $('#third_row').show();
    //   alert("aaa");
    // }else if(document.getElementById("control_kind").value == "1") {
    //   alert("bbb");
    //
    //   $('#third_row').show();
    // }else {
    //   alert("ccc")
    // }
    // document.getElementById("control_kind").options[0].selected = true;
    // $('#list_one').show();
    // document.getElementById("control_kind").options[1].selected = true;
    // $('#third_row').show();
  // }
});
