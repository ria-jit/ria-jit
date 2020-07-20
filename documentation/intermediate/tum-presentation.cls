%%
% TUM Corporate Design Presentation Slides
% Michael Riesch (michael.riesch@tum.de)
% Based on the templates from https://www.tum.de/cd
%
% Template for presentation slides
%
%%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{tum-presentation}[2018/10/080 TUM Corporate Design Slides v0.3]

% use widescreen?
\newif\ifwidescreen
\widescreentrue

% show navigation symbols?
\newif\ifnavsym
\navsymfalse

% show threeliner?
\newif\ifthreeliner
\threelinertrue

% four pages on one?
\newif\iffourpp
\fourppfalse

% two pages on one?
\newif\iftwopp
\twoppfalse

% process options
\DeclareOption{4to3}{\widescreenfalse}
\DeclareOption{16to9}{\widescreentrue}
\DeclareOption{2on1}{%
  \twopptrue
  \PassOptionsToClass{handout}{beamer}
}
\DeclareOption{4on1}{%
  \fourpptrue
  \PassOptionsToClass{handout}{beamer}
}
\DeclareOption{handout}{\PassOptionsToClass{handout}{beamer}}
\DeclareOption{navsym}{\navsymtrue}
\DeclareOption{nothreeliner}{\threelinerfalse}
\DeclareOption{german}{\PassOptionsToPackage{german}{tum-base}}
\DeclareOption{english}{\PassOptionsToPackage{english}{tum-base}}
\ProcessOptions\relax

% footline text
\newcommand{\insertfootline}{~}
\newcommand{\footline}[1]{\renewcommand{\insertfootline}{#1}}

\ifwidescreen

\newcommand{\tumpaperwidth}{25.4cm}
\newcommand{\tumpaperheight}{14.2875cm}
\newcommand{\tumauthorblockwidth}{15cm}

% load base class and packages for 16to9
\LoadClass[t,hyperref={hidelinks,breaklinks},aspectratio=169]{beamer}
\RequirePackage[
  orientation=landscape,
  size=custom,
  width=25.4,
  height=14.2875,
  scale=0.5 % erzeugt 16pt Schriftgröße
]{beamerposter}
\newcommand{\PraesentationSchriftgroesseSehrGross}{\fontsize{25}{30}}
\newcommand{\PraesentationSchriftgroesseGross}{\fontsize{18}{27}}
\newcommand{\PraesentationSchriftgroesseNormal}{\fontsize{14}{21}}
\newcommand{\PraesentationSchriftgroesseKlein}{\fontsize{11}{17}}
\newcommand{\PraesentationSchriftgroesseDreizeiler}{\fontsize{7}{10}}
\newcommand{\PraesentationSchriftgroesseAufzaehlungszeichen}{\fontsize{10}{8}}

\else

\newcommand{\tumpaperwidth}{25.4cm}
\newcommand{\tumpaperheight}{19.05cm}

%% TODO: adjust
\newcommand{\tumauthorblockwidth}{15cm}

% load base class and packages for 4to3
\LoadClass[t,hyperref={hidelinks,breaklinks},aspectratio=43]{beamer}
\RequirePackage[
  orientation=landscape,
  size=custom,
  width=25.4,
  height=19.05,
  scale=0.63 % erzeugt 16pt Schriftgröße
]{beamerposter}

\newcommand{\PraesentationSchriftgroesseSehrGross}{\fontsize{30}{36}}
\newcommand{\PraesentationSchriftgroesseGross}{\fontsize{22}{33}}
\newcommand{\PraesentationSchriftgroesseNormal}{\fontsize{16}{29}}
\newcommand{\PraesentationSchriftgroesseKlein}{\fontsize{12}{18}}
\newcommand{\PraesentationSchriftgroesseDreizeiler}{\fontsize{8}{10}}
\newcommand{\PraesentationSchriftgroesseAufzaehlungszeichen}{\fontsize{10}{8}}

\fi

% load TUM base packages
\RequirePackage{tum-base}

% handle options 4on1 and 2on1
\RequirePackage{pgfpages}
% 2on1
\iftwopp
\pgfpagesuselayout{2 on 1}[a4paper, border shrink=5mm]
\fi
% 4on1
\iffourpp
\pgfpagesuselayout{4 on 1}[a4paper, border shrink=5mm, landscape]
\fi

% load biblatex
\usepackage[style=authoryear,firstinits=true,doi=false,isbn=false,
  url=false,backend=biber]{biblatex}

% set dimensions
\newcommand{\PraesentationSeitenrand}{8.9mm}
\newcommand{\tumframemargin}{8.9mm}
\newcommand{\tumtopmargin}{21pt}
\newcommand{\tumheadline}{25pt}
\newcommand{\tumheadsep}{16pt}
\newcommand{\tumfootline}{30pt}

% set beamer fonts
\setbeamerfont{frametitle}{size=\PraesentationSchriftgroesseSehrGross}
\setbeamerfont{framesubtitle}{size=\PraesentationSchriftgroesseNormal}
\setbeamerfont{author}{size=\PraesentationSchriftgroesseNormal}
\setbeamerfont{institute}{size=\PraesentationSchriftgroesseKlein}
\setbeamerfont{date}{size=\PraesentationSchriftgroesseNormal}
\setbeamerfont{footline}{size=\PraesentationSchriftgroesseKlein}
\setbeamerfont{headline}{size=\PraesentationSchriftgroesseDreizeiler}
\setbeamerfont{footnote}{size=\PraesentationSchriftgroesseKlein}
\setbeamerfont{itemize/enumerate subbody}{size=\normalsize}

% custom color themes
\newcommand{\tumlogo}{tum-resources/images/Universitaet_Logo_RGB.pdf}
% set colors
\newcommand{\PraesentationFarbschemaStandard}{
  \setbeamercolor*{background canvas}{}
  \setbeamercolor*{alerted text}{fg=TUMOrange}
  \setbeamercolor*{bibliography entry author}{fg=TUMBlau}
  \setbeamercolor*{bibliography entry note}{fg=TUMBlau}
  \setbeamercolor*{caption name}{fg=TUMBlau}
  \setbeamercolor*{headline}{fg=TUMBlau}
  \setbeamercolor*{footline}{fg=black}
  \setbeamercolor*{frametitle}{fg=black}
  \setbeamercolor*{author}{fg=TUMBlau}
  \setbeamercolor*{navigation symbols}{fg=black}
  \setbeamercolor*{normal text}{fg=black}
  \setbeamercolor*{itemize/enumerate body}{fg=black}
  \setbeamercolor*{itemize item}{fg=black}
  \setbeamercolor*{itemize subitem}{fg=black}
  \setbeamercolor*{enumerate item}{fg=TUMBlau}
  \setbeamercolor*{enumerate subitem}{fg=TUMBlau}
  \setbeamercolor*{section in toc}{fg=TUMBlau}

  \setbeamercolor*{block title}{fg=white,bg=TUMBlau}
  \setbeamercolor*{block body}{fg=TUMBlau,bg=white}
  \setbeamercolor*{block title alerted}{fg=white,bg=TUMOrange}
  \setbeamercolor*{block body alerted}{fg=TUMOrange,bg=white}
  \setbeamercolor*{block title example}{fg=white,bg=TUMGruen}
  \setbeamercolor*{block body example}{fg=TUMGruen,bg=white}
  \renewcommand{\tumlogo}{tum-resources/images/Universitaet_Logo_RGB.pdf}
}

\newcommand{\PraesentationFarbschemaWeissBlau}{%
  \setbeamercolor*{background canvas}{bg=TUMBlauDunkel}%
  \setbeamercolor*{alerted text}{fg=TUMOrange}
  \setbeamercolor*{bibliography entry author}{fg=white}
  \setbeamercolor*{bibliography entry note}{fg=white}
  \setbeamercolor*{caption name}{fg=white}
  \setbeamercolor*{headline}{fg=white}
  \setbeamercolor*{footline}{fg=white}
  \setbeamercolor*{frametitle}{fg=white}
  \setbeamercolor*{author}{fg=white}
  \setbeamercolor*{navigation symbols}{fg=white}
  \setbeamercolor*{normal text}{fg=white}
  \setbeamercolor*{itemize/enumerate body}{fg=white}
  \setbeamercolor*{itemize item}{fg=white}
  \setbeamercolor*{itemize subitem}{fg=white}
  \setbeamercolor*{enumerate item}{fg=white}
  \setbeamercolor*{enumerate subitem}{fg=white}
  \setbeamercolor*{section in toc}{fg=white}
  \renewcommand{\tumlogo}{tum-resources/images/Universitaet_Logo_weiss.pdf}
}

\newcommand{\PraesentationFarbschemaWeissSchwarz}{%
  \setbeamercolor*{background canvas}{bg=black}%
  \setbeamercolor*{alerted text}{fg=TUMOrange}
  \setbeamercolor*{bibliography entry author}{fg=white}
  \setbeamercolor*{bibliography entry note}{fg=white}
  \setbeamercolor*{caption name}{fg=white}
  \setbeamercolor*{headline}{fg=white}
  \setbeamercolor*{footline}{fg=white}
  \setbeamercolor*{frametitle}{fg=white}
  \setbeamercolor*{author}{fg=white}
  \setbeamercolor*{navigation symbols}{fg=white}
  \setbeamercolor*{navigation symbols}{fg=white}
  \setbeamercolor*{normal text}{fg=white}
  \setbeamercolor*{itemize/enumerate body}{fg=white}
  \setbeamercolor*{itemize item}{fg=white}
  \setbeamercolor*{itemize subitem}{fg=white}
  \setbeamercolor*{enumerate item}{fg=white}
  \setbeamercolor*{enumerate subitem}{fg=white}
  \setbeamercolor*{section in toc}{fg=white}
  \renewcommand{\tumlogo}{tum-resources/images/Universitaet_Logo_weiss.pdf}
}

%\PraesentationFarbschemaWeissSchwarz
%\PraesentationFarbschemaWeissBlau
\PraesentationFarbschemaStandard

% clear navigation symbols, use custom version later
\setbeamertemplate{navigation symbols}{}

% custom frame title
\defbeamertemplate*{frametitle}{tum-default}{
  \raggedright
  \usebeamerfont{frametitle}
  \usebeamercolor[fg]{frametitle}
  \insertframetitle\par
  \ifx\insertframesubtitle\@empty
  \else{
    \usebeamerfont{framesubtitle}
    \usebeamercolor[fg]{framesubtitle}
    \insertframesubtitle\par
  }
  \fi
}

% custom headline
\defbeamertemplate*{headline}{tum-dreizeiler}{
  % navigation symbols (if enabled)
  \begin{beamercolorbox}[wd=\paperwidth, right]{}
    \vbox to\tumtopmargin{
      \ifnavsym
      \hbox{
        \usebeamercolor[fg]{navigation symbols}
        \insertframenavigationsymbol
        \insertsubsectionnavigationsymbol
        \insertsectionnavigationsymbol
      }
      \fi
    }
  \end{beamercolorbox}
  \leavevmode
  % threeliner (if enabled) + logo
  \begin{beamercolorbox}[wd=\tumframemargin, left]{}
    ~
  \end{beamercolorbox}
  \begin{beamercolorbox}[wd=0.65\textwidth, ht=\tumheadline, left, dp=0pt]{}
    \ifthreeliner
    \vbox to\tumheadline{
      \thegroup\par\vfill
      \thedepartment\par\vfill
      \theuniversity\par
    }
    \fi
  \end{beamercolorbox}
  \hfill
  \begin{beamercolorbox}[wd=0.25\textwidth, right]{}
    \includegraphics[height=\tumheadline]{\tumlogo}
  \end{beamercolorbox}
  \begin{beamercolorbox}[wd=\tumframemargin, left]{}
    ~
  \end{beamercolorbox}
  \vskip\tumheadsep
}

% custom footline
\defbeamertemplate*{footline}{tum-default}{%
  \leavevmode
  \begin{beamercolorbox}[wd=0.8\paperwidth]{}
    \hspace*{\PraesentationSeitenrand}
    \ifnum\insertpagenumber=1\strut\else\insertfootline\fi
  \end{beamercolorbox}
  \hfill
  \begin{beamercolorbox}[wd=.1\paperwidth, right]{}
    \ifnum\insertpagenumber=1\strut\else\insertframenumber{}\fi
    \hspace*{\PraesentationSeitenrand}
  \end{beamercolorbox}
  \vskip3.25mm
}

% common title page content
\newcommand{\PraesentationTitelseiteInhalt}{%
  \frametitle{\inserttitle}
  \framesubtitle{\insertsubtitle}
  \bigskip
  \begin{beamercolorbox}[wd=\tumauthorblockwidth]{}
    \usebeamerfont{author}\usebeamercolor[fg]{author}
    \insertauthor\par
  \end{beamercolorbox}
  \bigskip
  \begin{beamercolorbox}[wd=\tumauthorblockwidth]{}
    \usebeamerfont{institute}\usebeamercolor[fg]{institute}
    \insertinstitute\par
  \end{beamercolorbox}
  \bigskip
  \begin{beamercolorbox}[wd=\tumauthorblockwidth]{}
    \usebeamerfont{date}\usebeamercolor[fg]{date}\insertdate\par
  \end{beamercolorbox}
}

% custom title page Uhrenturm
\defbeamertemplate*{title page}{tum-uhrenturm}{
  \PraesentationTitelseiteInhalt
  \begin{textblock*}{10.82cm}[1, 1](\paperwidth, \paperheight)
    \includegraphics{tum-resources/images/TUM_Uhrenturm.png}
  \end{textblock*}
}

% custom title page Flaggen
\defbeamertemplate{title page}{tum-flaggen}{
  \PraesentationTitelseiteInhalt

  asdf
  %\begin{textblock*}{\paperwidth}[1, 1](\paperwidth, \paperheight)
  \begin{textblock*}{\paperwidth}[0, 1](-\tumframemargin, \paperheight-\tumframemargin)
    %\includegraphics[min width=\paperwidth, max width=\paperheight,
    %  min totalsize={\paperwidth}{\paperheight}, keepaspectratio,
    %  center]{tum-resources/images/Universitaet_Flaggen.jpg}
  \end{textblock*}
}

% custom itemize
\setbeamertemplate{itemize items}[circle]
\setbeamertemplate{itemize subitem}{$-$}
\setbeamertemplate{enumerate items}{\arabic{enumi}}

\setlength{\itemsep}{0pt}
\addtolength{\itemindent}{-1ex}

% compatibility to old template versions
\newenvironment{PraesentationAufzaehlung}{\begin{itemize}}{\end{itemize}}
