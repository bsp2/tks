% simplemind postprocess
% smmx_to_markdown
% 05-Jan-2020

$(var:header)

# Table of Contents
$(buf!toc)



simplemind postprocess
======================


## fileformat analysis


### zip
   - XML for text
   - PNG images


### parser
   - *from*: [markdown converter](#markdown-converter), [emacs org-mode converter](#emacs-org-mode-converter)
   - concatenate text notes that begin with #
   - text nodes to headers
      - notes to paragraph text
   - list style subtrees to lists
   - image nodes
   - calendar dates
     Date: 01-01-2020
   - links
      - cross references
        *to*: [lists](#lists)
      - URLs


markdown converter
==================
   - *to*: [parser](#parser)


emacs org-mode converter
========================
   - *to*: [parser](#parser)


lists
=====
   - *from*: [cross references](#cross-references)


$(buf!footnote)
$(buf!w3validator)

Document created on $(var:localdatetime)
].create
