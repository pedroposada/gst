<ANSI-MAC><?php print "\r" ?>
<?php foreach ($posts as $key => $value) { ?>
<ParaStyle:isawyou HEADLINE><?php print $value['title'] ?><?php print "\r" ?>
<ParaStyle:isawyou INFO>I SAW A: <CharStyle:isawyou <?php print $value['isawa_gender'] ?>><?php print $value['isawa_char'] ?><CharStyle:> I AM A: <CharStyle:isawyou <?php print $value['iama_gender'] ?>><?php print $value['iama_char'] ?><CharStyle:><?php print "\r" ?>
<ParaStyle:isawyou INFO>WHEN: <?php print $value['date'] ?><?php print "\r" ?>
<ParaStyle:isawyou INFO>WHERE: <?php print $value['where'] ?><?php print "\r" ?>
<ParaStyle:isawyou BODY><?php print $value['body']; ?><?php print "\r" ?>
<?php } ?>