$(document).ready(function(e){
  $("#save_device").click(function(e){
    saveDevice();
  });
  // $("#save_device_add").click(function(e){
  //   saveDeviceAdd();
  // });
});

function saveDevice(){
  var k = document.getElementById("kind").value;
  var i = document.getElementById("id").value;
  var po = document.getElementById("position").value;
  var de = document.getElementById("designation").value;
  var re = document.getElementById("require").value;
  if(k == "" || i == "" || po == "" || de == "" || re == "" ){
    alert("数据不能为空！");
    return false;
    }else{
      $.ajax({
        url:"/goform/dev_add",
      data:{
          kind:$("#kind").val(),
          id:$("#id").val(),
          position:$("#position").val(),
          designation:$("#designation").val(),
          require:$("#require").val(),
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

// function saveDeviceAdd(){
//   var k1 = document.getElementById("kind1").value;
//   // var k2 = document.getElementById("kind2").value;
//   // var k3 = document.getElementById("kind3").value;
//   // var k4 = document.getElementById("kind4").value;
//   // var k5 = document.getElementById("kind5").value;
//
//   var i1 = document.getElementById("id1").value;
//   // var i2 = document.getElementById("id2").value;
//   // var i3 = document.getElementById("id3").value;
//   // var i4 = document.getElementById("id4").value;
//   // var i5 = document.getElementById("id5").value;
//
//   var po1 = document.getElementById("postion1").value;
//   // var po2 = document.getElementById("postion2").value;
//   // var po3 = document.getElementById("postion3").value;
//   // var po4 = document.getElementById("postion4").value;
//   // var po5 = document.getElementById("postion5").value;
//
//   var de1 = document.getElementById("designation1").value;
//   // var de2 = document.getElementById("designation2").value;
//   // var de3 = document.getElementById("designation3").value;
//   // var de4 = document.getElementById("designation4").value;
//   // var de5 = document.getElementById("designation5").value;
//
//   var re1 = document.getElementById("require1").value;
//   // var re2 = document.getElementById("require2").value;
//   // var re3 = document.getElementById("require3").value;
//   // var re4 = document.getElementById("require4").value;
//   // var re5 = document.getElementById("require5").value;
//
//   if(k1 == "" || i1 == "" || po1 == "" || de1 == "" || re1 == "" ){
//     alert("数据不能为空！");
//     return false;
//     }else{
//       $.ajax({
//         url:"/goform/login_server",
//       data:{
//           kind1:$("#kind1").val(),
//           kind2:$("#kind2").val(),
//           kind3:$("#kind3").val(),
//           kind4:$("#kind4").val(),
//           kind5:$("#kind5").val(),
//           id1:$("#id1").val(),
//           id2:$("#id2").val(),
//           id3:$("#id3").val(),
//           id4:$("#id4").val(),
//           id5:$("#id5").val(),
//           position1:$("#position1").val(),
//           position2:$("#position2").val(),
//           position3:$("#position3").val(),
//           position4:$("#position4").val(),
//           position5:$("#position5").val(),
//           designation1:$("#designation1").val(),
//           designation2:$("#designation2").val(),
//           designation3:$("#designation3").val(),
//           designation4:$("#designation4").val(),
//           designation5:$("#designation5").val(),
//           require1:$("#require1").val(),
//           require2:$("#require2").val(),
//           require3:$("#require3").val(),
//           require4:$("#require4").val(),
//           require5:$("#require5").val(),
//         },
//         type:"POST",
//         dataType:"text",
//         async: true,
//         // contentType:"application/x-www-form-urlencoded; charset=UTF-8"
//         success: function(data){    //    alert后台的返回值
//                alert("访问成功");
//            },
//         error: function(){
//                alert('访问失败');
//            }
//       });
//       return true;
//     }
// }
