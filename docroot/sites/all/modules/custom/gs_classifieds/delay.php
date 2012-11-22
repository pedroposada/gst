<html>
<head>
<script type="text/javascript">
<!--
function delayer(){
  	  var param = gup2('dest');
alert('parameter is: ' + param);
    window.location = "../../../../../" + param
}

function gup2( name ){
      
  name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");  
      
  var regexS = "[\\?&]"+name+"=([^&#]*)";  
      
  var regex = new RegExp( regexS );  
      
  var results = regex.exec( window.location.href ); 
      
  if( results == null )    return "";  
  else    return results[1];
}

//-->
</script>
</head>
<body onLoad="setTimeout('delayer()', 5000)">
<h2>Prepare to be redirected!</h2>
<p>This page is a time delay redirect, please update your bookmarks to our new 
location!</p>

</body>
</html>