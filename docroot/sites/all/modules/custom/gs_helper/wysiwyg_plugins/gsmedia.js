/**
 *  @file
 *  Attach Media WYSIWYG behaviors.
 */

(function($) {

	Drupal.gsmedia = Drupal.gsmedia || {};

	// Define the behavior.
	Drupal.wysiwyg.plugins.gsmedia = {

		/**
		 * Execute the button.
		 */
		invoke : function(data, settings, instanceId) {
			// for debug
			console.log("clicked gsmedia button");

			// create object once; to attach dialog to it
			var myDialog = $('<div class="gsmedia-browser-contents" />');

			// configure modal dialog for images
			myDialog.dialog({
				title : "Select Media",
				autoOpen : false,
				height : 600,
				width : 600,
				modal : true,
				buttons : {
					// 'Insert' : function() {
						// Drupal.wysiwyg.plugins.gsmedia.insertMediaFile(Drupal.wysiwyg.instances[instanceId]);
						// $(this).dialog("close");
					// },
					'Cancel' : function() {
						$(this).dialog("close");
					}
				},
				open : function(event, ui) {
					var nid = Drupal.settings.gs_helper.nid;
					if (nid == undefined) {
						// console.log("nid is undefined");
						myDialog.html("Please add media to this node first...");
					} else {
						// console.log("nid is " + nid);
						myDialog.html("Loading...");
						$.get('/gs/ajax/media-browser/' + nid, function(data){
			      			myDialog.html(data).find(".gsmedia-item-wrapper").bind('click', function(){
			      				var fid = this.id;
			      				var cutline = $(".cutline", this).html();
			      				var credit = $(".credit", this).html();
			      				console.log("media item " + fid + " clicked.");
								$.get('/gs/ajax/inline-media/' + fid, function(data){
									var wysiwygInstance = Drupal.wysiwyg.instances[instanceId];
									var media = $(data);
									if (cutline.length) {
										$(".cutline", media).html(cutline);
									} else {
										$(".cutline", media).remove();
									}
									if (credit.length) {
										$(".credit", media).html(credit);
									} else {
										$(".credit", media).remove();
									}
									wysiwygInstance.insert(media.html() + "<p></p>");
				      				myDialog.dialog("close");
								});
			      			});
			      		});
					}
				},
				close : function(event, ui) {
					$(this).dialog("destroy");
					$(this).remove();
				},
			});

			// open modal dialog
			myDialog.dialog("open");
		},
		
		/**
		 * Insert media in text area
		 */
		insertMediaFile : function(media, wysiwygInstance) {
			wysiwygInstance.insert("gsmedia insert test.");
		},
	};

})(jQuery);
