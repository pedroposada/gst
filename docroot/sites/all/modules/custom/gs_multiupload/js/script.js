(function($){
	var dropbox = $('#dropbox'),
		message = $('.message', dropbox);
	var fids = [];
	$.data(document,'fids',fids);
	
	dropbox.filedrop({
		fallback_id: 'fallback_fileupload',  	// an identifier of a standard file input element
		paramname:'pic', 				// The name of the $_FILES entry:
	 	maxfilesize: Drupal.settings.gs_multiupload.maxfilesize,    			// max file size in MBs
    	maxfiles: Drupal.settings.gs_multiupload.maxfiles,
		// queuefiles: Drupal.settings.gs_multiupload.queuefiles,   				// Control how many uploads are attempted in parallel (ignores maxfiles setting)
		url: '/gs/multiupload/' + Drupal.settings.gs_multiupload.nid + '/add',
		uploadFinished:function(i,file,response){
			if (response.errors) {
				alert(response.message);				
			} else {
				$.data(file).addClass('done');
				var fid = response.message;
				$.data(document,'fids').push(fid);
				console.log($.data(document,'fids'));
			}
			// response is the JSON object that post_file.php returns
			// console.log(response.status);
		},
    	error: function(err, file) {
			switch(err) {
				case 'BrowserNotSupported':
					showMessage('Your browser does not support HTML5 file uploads!');
					break;
				case 'TooManyFiles':
					alert('Too many files! Please select '+ Drupal.settings.gs_multiupload.maxfiles +' at most!');
					break;
				case 'FileTooLarge':
					alert(file.name + ' is too large! Please upload files up to 2mb.');
					break;
				default:
					break;
			}
		},
		// Called before each upload is started
		beforeEach: function(file){ 
			if(!file.type.match(/^image\//)){
				alert('Only images are allowed!');
				
				// Returning false will cause the
				// file to be rejected
				return false;
			}
		},
		uploadStarted:function(i, file, len){
			createImage(file);
		},
		progressUpdated: function(i, file, progress) {
			$.data(file).find('.progress').width(progress);
		}
	});
	
	var template = '<div class="preview">'+
						'<span class="imageHolder">'+
							'<img />'+
							'<span class="uploaded"></span>'+
						'</span>'+
						'<div class="progressHolder">'+
							'<div class="progress"></div>'+
						'</div>'+
					'</div>'; 
	
	
	function createImage(file){

		var preview = $(template), 
			image = $('img', preview);
			
		var reader = new FileReader();
		
		image.width = 100;
		image.height = 100;
		
		reader.onload = function(e){
			
			// e.target.result holds the DataURL which
			// can be used as a source of the image:
			
			image.attr('src',e.target.result);
		};
		
		// Reading the file as a DataURL. When finished,
		// this will trigger the onload function above:
		reader.readAsDataURL(file);
		
		message.hide();
		preview.appendTo(dropbox);
		
		// Associating a preview container
		// with the file, using jQuery's $.data():
		
		$.data(file,preview);
	}

	function showMessage(msg){
		message.html(msg);
	}

})(jQuery);