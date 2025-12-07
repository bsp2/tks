Emacs / org-mode extension for TKS cellular notebooks / literate programming

# .emacs configuration
~~~~~
; org mode: eval languages (M-x org-babel-execute-src-block)
(require 'ob-tks)
(require 'ob-tks-rs)
;;(when (eq system-type 'windows-nt)
;;  (setq org-babel-tks-command "\"c:/Program Files/tks/tks.exe\""))
(cond
 ((eq system-type 'darwin)
  (setq org-babel-tks-command "/usr/local/bin/tks")
  )
 ((eq system-type 'gnu/linux)
  (setq org-babel-tks-command "tks")
  )
 ((eq system-type 'windows-nt)
  (setq org-babel-tks-command "\"c:/Program Files/tks/tks.exe\"")
  )
 )
(add-to-list 'org-src-lang-modes (cons "tks" 'java))
(add-to-list 'org-src-lang-modes (cons "tks-rs" 'java))
(setq org-src-fontify-natively t)
(setq org-confirm-babel-evaluate nil)   ; potentially dangerous (disable confirmation)
(setq org-babel-python-command "python3")
(add-hook 'org-babel-after-execute-hook 'org-display-inline-images)
(org-babel-do-load-languages
 'org-babel-load-languages
 '((emacs-lisp . t)
   (R . t)
   (C . t)
   ;(cpp . t)
   (calc . t)
   (python . t)
   (perl . t)
   (ruby . t)
   (tks . t)
   (tks-rs . t)
   (shell . t)
   ))
(global-set-key (kbd "C-x C-c") 'org-babel-execute-buffer)
~~~~~

# Examples
See [cellular.org](../test_rs/org/cellular.org), [literate.org](../test_rs/org/literate.org)
