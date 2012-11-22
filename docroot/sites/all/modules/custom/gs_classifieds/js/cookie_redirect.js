(function($) {
	// $(document).ready(function() {
      
	  var cookiename = 'adult' + "=";  
  
      var ca = document.cookie.split(';');  
  
	  var param = window.location.pathname;
	  var param2 = param.substring(1);
	  var count = 0;
      for(var i=0;i < ca.length;i++)  
      {  
  
        var c = ca[i];  
  
        while (c.charAt(0)==' ') c = c.substring(1,c.length);  
   
        if (c.indexOf(cookiename) == 0) {
		  count = 1;
		}
      }

      if (count == 1) {
		// alert('cookie is set!');
	  
	  }	else {
	    window.location = "/adult-services-disclaimer?dest=" + param2;  //do redirect
	  }  
  
	// });
})(jQuery); 

function gup( name ){
      
  name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");  
      
  var regexS = "[\\?&]"+name+"=([^&#]*)";  
      
  var regex = new RegExp( regexS );  
      
  var results = regex.exec( window.location.href ); 
      
  if( results == null )    return "";  
  else    return results[1];
}

