% local_link_test
% smmx_to_markdown
% 05-Jan-2020

$(var:header)

# Table of Contents
$(buf!toc)



local_link_test
===============


## link to a local file
Link: [F:\tmp\bla.txt](F:\tmp\bla.txt) *(file)*



## link to web page
Link: [http://www.google.de](http://www.google.de) *(url)*



## link to cloud file
Link: [./relations.smmx](./relations.smmx) *(cloud)*



$(buf!footnote)
$(buf!w3validator)

Document created on $(var:localdatetime)
].create
