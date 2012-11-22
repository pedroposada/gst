var tweeter = jQuery("#tweeter"),
    tweeter_height = 0,
    tweets_height = 0,
    tweet_qtype,
    tweet_qstr,
    tweet_loop_max = 20,
    tweet_radius = 150,
    tweet_count = 10,
    tweets_to_show = 5,
    loopCount = 0;

function showTweets(qtype, qstr, srad, init) {
  var url = '',
      slugs;
  
  tweet_qtype = qtype;
  tweet_qstr = qstr;
  
  if (init == null) {
    init = true;
  }
  
  switch (qtype) {
    case 'user':
      url = 'https://api.twitter.com/1/statuses/user_timeline.json?callback=?&screen_name=' + qstr + '&count=' + tweet_count;
      break;
    case 'search':
      if(srad == null) {
        url = 'http://search.twitter.com/search.json?callback=?&rpp=' + tweet_count + '&q=' + qstr;
      }
      else {
        // rrp = The number of tweets to return per page, up to a max of 100.
        url = 'http://search.twitter.com/search.json?callback=?&rpp=' + tweet_count + '&geocode=' + encodeURIComponent('49.263637,-123.143443,' + srad + 'km') + '&q=' + encodeURIComponent(qstr);
      }
      break;
    case 'list':
      // per_page = Specifies the number of results to retrieve per page.
      slugs = qstr.split('/')
      url = 'https://api.twitter.com/1/lists/statuses.json?callback=?&owner_screen_name=' + slugs[0] + '&slug=' + slugs[1] + '&per_page=' + tweet_count;
      break;
    default:
      url = 'https://api.twitter.com/1/statuses/user_timeline.json?callback=?&screen_name=georgiastraight&count=' + tweet_count;
  }
  
  jQuery.getJSON(url, function(data) {
    var i = 0,
        l = data.length,
        str = '';
    
    if (qtype == 'user') {
      data.reverse();
      
      if (jQuery('.twitter-user:first').length == 0){
        tweeter.before('<div class="twitter-user"><img src="' + data[0].user.profile_image_url + '" class="twitter-avatar" alt=""><p><a href="http://twitter.com/' + qstr + '" class="twitter-handle" target="_blank">@' + qstr + '</a>' + autoLink(data[0].user.description) + '</p></div>');
      }
      
      for (i = 0; i < l; i++) {
        // USER OUTPUT
        str = '<div id="tweet-id-' + data[i].id_str + '" class="tweet">';
        str = str + '<p>' + autoLink(data[i].text);
        str = str + '<em><a href="http://twitter.com/' + data[i].user.screen_name + '/status/' + data[i].id_str + '" time="' + data[i].created_at + '" class="tweet-timestamp" target="_blank">' + timeAgo(data[i].created_at) + '</a> &middot; <a href="http://twitter.com/?status=@' + data[i].user.screen_name + '%20&amp;in_reply_to_status_id=' + data[i].id_str + '&amp;in_reply_to=' + data[i].user.screen_name + '"  class="tweet-reply" target="_blank">reply</a> &middot; <a href="http://twitter.com/intent/retweet?tweet_id=' + data[i].id_str + '"  class="tweet-rt" target="_blank">retweet</a> &middot; <a href="http://twitter.com/intent/favorite?tweet_id=' + data[i].id_str + '"  class="tweet-fav" target="_blank">favorite</a></em></p></div>';
        
        tweeter.append(str);
        
        if (i < tweets_to_show && init === true) {
          jQuery("#tweeter .tweet:last").toggleClass('tweet-show');
        }
        else {
          jQuery("#tweeter .tweet:last").hide();
        }
      }
    }
    else if (qtype == 'list') {
      data.reverse();
      
      for (i = 0; i < data.length; i++) {
        // LIST OUTPUT
        str = "<div id=\"tweet-id-" + data[i].id_str + "\" class=\"tweet\">";
        str = str + "<a href=\"http://twitter.com/" + data[i].user.screen_name + "\" class=\"tweet-avatar\" target=\"_blank\"><img src=\"" + data[i].user.profile_image_url + "\" alt=\"" + data[i].user.screen_name + "'s profile\"></a>";
        str = str + '<p><a class="tweet-user" href="http://twitter.com/' + data[i].user.screen_name + '" target="_blank">' + data[i].user.screen_name + '</a> ';
        str = str + autoLink(data[i].text);
        str = str + '<em><a href="http://twitter.com/' + data[i].user.screen_name + '/status/' + data[i].id_str + '" time="' + data[i].created_at + '" class="tweet-timestamp" target="_blank">' + timeAgo(data[i].created_at) + '</a> &middot; <a href="http://twitter.com/?status=@' + data[i].user.screen_name + '%20&amp;in_reply_to_status_id=' + data[i].id_str + '&amp;in_reply_to=' + data[i].user.screen_name + '"  class="tweet-reply" target="_blank">reply</a> &middot; <a href="http://twitter.com/intent/retweet?tweet_id=' + data[i].id_str + '"  class="tweet-rt" target="_blank">retweet</a> &middot; <a href="http://twitter.com/intent/favorite?tweet_id=' + data[i].id_str + '"  class="tweet-fav" target="_blank">favorite</a></em></p></div>';
        
        tweeter.append(str);
        
        if (i < tweets_to_show && init === true) {
          jQuery("#tweeter .tweet:last").toggleClass('tweet-show');
        }
        else {
          jQuery("#tweeter .tweet:last").hide();
        }
      }
    }
    else if (qtype == 'search') {
      data.results.reverse();
      
      for(i = 0; i < data.results.length; i++) {
        // SEARCH OUTPUT
        str = '<div id="tweet-id-' + data.results[i].id + '" class="tweet">';
        str = str + '<a href="http://twitter.com/' + data.results[i].from_user + '" class="tweet-avatar" target="_blank"><img src="' + data.results[i].profile_image_url + '" alt="' + data.results[i].from_user + '\'s profile"></a>';
        str = str + '<p><a class="tweet-user" href="http://twitter.com/' + data.results[i].from_user + '" target="_blank">' + data.results[i].from_user + '</a> ';
        str = str + autoLink(data.results[i].text);
        str = str + '<em><a href="http://twitter.com/' + data.results[i].from_user + '/status/' + data.results[i].id_str + '" time="' + data.results[i].created_at + '" class="tweet-timestamp" target="_blank">' + timeAgo(data.results[i].created_at) + '</a> &middot; <a href="http://twitter.com/?status=@' + data.results[i].from_user + '%20&amp;in_reply_to_status_id=' + data.results[i].id_str + '&amp;in_reply_to=' + data.results[i].from_user + '" class="tweet-reply" target="_blank">reply</a></em></p></div>';
        
        tweeter.append(str);
        
        if (i < tweets_to_show && init === true) {
          jQuery("#tweeter .tweet:last").toggleClass('tweet-show');
        }
        else {
          jQuery("#tweeter .tweet:last").hide();
        }
      }
    }
    
    if (init === true) {
      jQuery(tweeter).children('.tweet').each(function () {
        tweets_height = tweets_height + jQuery(this).outerHeight(true);
      });
      
      if (tweeter_height == 0) {
        tweeter_height = tweeter.height();
        tweets_height = Math.round(tweets_height / 2);
        
        if (tweeter_height < tweets_height) {
          tweeter_height = tweets_height;
        }
        
        tweeter.height(tweeter_height);
      }
    }
  }); //END getJSON()  
} //END showTweets()

function autoLink(text){
  // Link links, emails, #hastags and @handles
  return text.replace(/([a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4})/, '<a href="mailto:$1" target="_blank">$1</a>').replace(/(http:\/\/[\w\.\/]+)/, '<a href="$1" target="_blank">$1</a>').replace(/(^|\s|\()@([A-Za-z0-9_-]+)/g, '$1<a href="http://twitter.com/$2" target="_blank">@$2</a>').replace(/(^|\s|\()#([A-Za-z0-9_-]+)/g, '$1<a href="http://twitter.com/search?q=%23$2" target="_blank">#$2</a>');
}

function tweetScroll(loop_amount){
  if(typeof loop_amount == "undefined") {
    loop_amount = tweet_loop_max;
  }
  
  loopCount++;
  
  if(jQuery("#tweeter .tweet-stale").length) {
    jQuery("#tweeter .tweet-stale:last").remove();
    jQuery("#tweeter .tweet:last").insertBefore("#tweeter .tweet:first").slideDown("slow").toggleClass('tweet-show');
    jQuery("#tweeter .tweet-show:last").hide().toggleClass('tweet-show');
  }
  else {
    jQuery("#tweeter .tweet:last").insertBefore("#tweeter .tweet:first").slideDown("slow").toggleClass('tweet-show');
    jQuery("#tweeter .tweet-show:last").hide().toggleClass('tweet-show');
  }
  
  if(loopCount == loop_amount) {
    loopCount = 0;  
    jQuery("#tweeter .tweet").addClass("tweet-stale");
    showTweets(tweet_qtype, tweet_qstr, null, false);
  }
}

/**
* Relative Time Calculator
* @param {string} twitter date string returned from Twitter API
* @return {string} relative time like "2 minutes ago"
*/
function timeAgo(dateString) {
  var rightNow = new Date(),
      then = new Date(dateString),
      browser = function() {
        var ua = navigator.userAgent;
        return {ie: ua.match(/MSIE\s([^;]*)/)};
      };
  
  if (browser.ie) {
    // IE can't parse these crazy Ruby dates
    then = Date.parse(dateString.replace(/( \+)/, ' UTC$1'));
  }
  
  var diff = rightNow - then,
      second = 1000,
      minute = second * 60,
      hour = minute * 60,
      day = hour * 24,
      week = day * 7;
  
  if (isNaN(diff) || diff < 0) {
    // return blank string if unknown
    return '';
  }
  
  if (diff < second * 2) {
    // within 2 seconds
    return 'right now';
  }
  
  if (diff < minute) {
    return Math.floor(diff / second) + ' seconds ago';
  }
  
  if (diff < minute * 2) {
    return 'about 1 minute ago';
  }
  
  if (diff < hour) {
    return Math.floor(diff / minute) + ' minutes ago';
  }
  
  if (diff < hour * 2) {
    return 'about 1 hour ago';
  }
  
  if (diff < day) {
    return  Math.floor(diff / hour) + ' hours ago';
  }
  
  if (diff > day && diff < day * 2) {
    return 'yesterday';
  }
  
  if (diff < day * 365) {
    return Math.floor(diff / day) + ' days ago';
  }
  
  else {
    return 'over a year ago';
  }
}