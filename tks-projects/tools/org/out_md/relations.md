% relations
% smmx_to_markdown
% 12-Jan-2020

$(var:header)

# Table of Contents
$(buf!toc)



relations
=========
   - **to**: [double arrow](#double-arrow)


## node A
   - **from**: [node C](#node-c), [thin solid](#thin-solid)


### node B
   - **to**: [node C](#node-c), [node D](#node-d)


## node C
   - **from**: [node B](#node-b), [node D](#node-d)
   - **to**: [node A](#node-a), [cable dash](#cable-dash)


## node D
   - **from**: [double arrow](#double-arrow), [cable dash](#cable-dash), [thin solid](#thin-solid), [node B](#node-b)
   - **to**: [node C](#node-c)


double arrow
============
   - **from**: [angled](#angled), [straight](#straight), [relations](#relations)
   - **to**: [node D](#node-d)


cable dash
==========
   - **from**: [node C](#node-c)
   - **to**: [node D](#node-d), [straight](#straight)


thin solid
==========
   - **to**: [node D](#node-d), [node A](#node-a)


angled
======
   - **from**: [straight](#straight)
   - **to**: [double arrow](#double-arrow)


straight
========
   - **from**: [cable dash](#cable-dash)
   - **to**: [double arrow](#double-arrow), [angled](#angled)


$(buf!footnote)
$(buf!w3validator)

Document created on $(var:localdatetime)
].create
