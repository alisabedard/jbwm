syn on
colo industry
set background=dark
set shiftwidth=4
set expandtab
set softtabstop=4
autocmd FileType make setlocal noexpandtab
autocmd FileType make setlocal shiftwidth=8
autocmd FileType make setlocal softtabstop=8
set visualbell
set colorcolumn=80
set helpheight=15
"set guifont=Monospace\ 10
set menuitems=10
set linespace=1
set guifont=-*-lucidatypewriter-medium-*-*-*-14-*-*-*-*-*-*-*
set guioptions=c
set autoread
set mouse=a
set smartindent
autocmd BufEnter *.c setf cpp
autocmd FileType c,cpp :setf cpp
autocmd FileType c,cpp :set cindent
set autoread
set cinoptions=>1s,e0,n0,f0,{0,}0,^0,L0,:0,=s,l1,b0,g0,h0,N0,p0,t0,i1s,+0,
   \c1s,C0,/0,(1s,u1s,U1,w1,W1s,k0,m1,M0,j1,J1,)100,*100,#0
