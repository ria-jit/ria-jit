% File: tumbeamer.sty Copyright (C) 2019 The LaTeX4EI Team
% Provides the TUM Corporate Design beamer class.
%
% It may be distributed and/or modified under the conditions of the
% LaTeX Project Public License (LPPL), either version 1.3c of this
% license or (at your option) any later version. The latest version
% of this license is in the file
%
%    https://www.latex-project.org/lppl.txt
%
% This file is part of the "tum-templates bundle" (The Work in LPPL)
% and all files in that bundle must be distributed together.
%
%
% ----------------------------------------------------------------------------
%
% The development version of the bundle can be found at
%
%    https://gitlab.lrz.de/latex4ei/tum-templates
%
% for those people who are interested.
%
% ----------------------------------------------------------------------------
%
% Version history:
% v1.0 2020/10/01 Initial version.
%
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{tumbeamer}[2020/10/01 v1.0]


% .:: Load required packages
% ----------------------------------------------------------------------------

\RequirePackage{pgfkeys}
\RequirePackage{pgfopts}
\RequirePackage{pgfpages}
\RequirePackage{tikz}
\RequirePackage{etoolbox}


% .:: Package options for TUMbeamer class
% ----------------------------------------------------------------------------

\newif\if@tum@widescreen@
\newif\if@tum@partpage@
\newif\if@tum@sectionpage@

\pgfqkeys{/TUM/beamer}{
  aspectratio/.is choice,
  aspectratio/169/.code = {
      \PassOptionsToClass{aspectratio=169, 11pt}{beamer}
      \@tum@widescreen@true
    },
  aspectratio/43/.code = {
      \PassOptionsToClass{aspectratio=43, 10pt}{beamer}
      \@tum@widescreen@false
    },
  handout/.is choice,
  handout/2on1/.code = {
      \PassOptionsToClass{handout}{beamer}
      \AtEndPreamble{
        % this has to be executed after the base class is loaded
        \pgfpagesuselayout{2 on 1}[a4paper, border shrink=5mm]
      }
    },
  handout/4on1/.code = {
      \PassOptionsToClass{handout}{beamer}
      \AtEndPreamble{
        % this has to be executed after the base class is loaded
        \pgfpagesuselayout{4 on 1}[a4paper, border shrink=5mm, landscape]
      }
    },
  partpage/.is if = @tum@partpage@,
  sectionpage/.is if = @tum@sectionpage@,
}

% process standard options
\pgfqkeys{/TUM/beamer}{aspectratio=169, sectionpage=true}

% process TUMbeamer class options
\ProcessPgfOptions{/TUM/beamer}


% .:: Load base class
% ----------------------------------------------------------------------------

% store classoptionslist
\let\@tum@classoptions@backup@\@classoptionslist

% pass options to beamer
\PassOptionsToClass{t, noamssymb, noamsthm}{beamer}
% load 'xcolor' package with 'colortbl' package for compatibility
\PassOptionsToClass{xcolor=table}{beamer}
% pass TUMbeamer class options down to beamer
\PassOptionsToClass{\@classoptionslist}{beamer}
\LoadClass{beamer}

% restore classoptionslist (because beamer cleared it)
\let\@classoptionslist\@tum@classoptions@backup@


% .:: Load TUM packages
% ----------------------------------------------------------------------------

\RequirePackage{tumbase}
\RequirePackage{tumlang}
\RequirePackage{tumcolors}
\RequirePackage{tumlogo}
\RequirePackage[rulewidth=0.5mm]{tumboxes}

\PassOptionsToPackage{scaled=0.94}{helvet}
\RequirePackage{tumfonts}


% .:: Basic setup
% ----------------------------------------------------------------------------

\newlength{\@tum@logoheight@}
\setlength{\@tum@logoheight@}{5.6mm}

\newlength{\@tum@vmargin@}
\setlength{\@tum@vmargin@}{\@tum@logoheight@}
\newlength{\@tum@hmargin@}
\setlength{\@tum@hmargin@}{\@tum@logoheight@}

% define beamer margins
\setbeamersize{
  text margin left = \@tum@hmargin@,
  text margin right = \@tum@hmargin@
}

% define resource files
\pgfqkeys{/TUM/beamer/resources}{
  flag image 169/.initial    = {resources/TUM_Flags_169.png},
  flag image 43/.initial     = {resources/TUM_Flags_43.png},
  tower image blue/.initial  = {resources/TUM_Tower.png},
  tower image white/.initial = {resources/TUM_Tower_white.png},
  % the link is updated later according to the background color
  tower image/.link = /TUM/beamer/resources/tower image blue,
}

% remove navigation symbols
\beamertemplatenavigationsymbolsempty

% ensure math is printed completely in serif font
% (avoid beamer mixing up letters from helvetica with serif math symbols)
\usefonttheme[onlymath]{serif}

% caption setup
\setbeamertemplate{caption}[numbered]
\setbeamertemplate{caption label separator}[colon]


% .:: Font setup
% ----------------------------------------------------------------------------

\setbeamerfont{title}{size=\LARGE, series=\bfseries}
\setbeamerfont{subtitle}{size=\large, series=\bfseries}
\setbeamerfont{author}{size=\normalsize, series=\bfseries}
\setbeamerfont{institute}{size=\footnotesize, series=\mdseries}
\setbeamerfont{date}{size=\normalsize, series=\mdseries}

\setbeamerfont{frametitle}{size=\Large, series=\bfseries}
\setbeamerfont{framesubtitle}{size=\normalsize, series=\bfseries}
\setbeamerfont{headline}{size=\scriptsize, series=\mdseries}
\setbeamerfont{footline}{size=\scriptsize, series=\mdseries}

\setbeamerfont{section name}{size=\LARGE, series=\bfseries}
\setbeamerfont{section title}{size=\huge, series=\bfseries}
\setbeamerfont{part name}{size=\LARGE, series=\bfseries}
\setbeamerfont{part title}{size=\Huge, series=\bfseries}

\setbeamerfont{itemize/enumerate body}{size=\normalsize}
\setbeamerfont{itemize/enumerate subbody}{size=\small}
\setbeamerfont{itemize/enumerate subsubbody}{size=\footnotesize}
\setbeamerfont{description item}{shape=\bfseries}

\setbeamerfont{section in toc}{size=\large}
\setbeamerfont{subsection in toc}{size=\normalsize}
\setbeamerfont{subsubsection in toc}{size=\small}
\setbeamerfont{section number projected}{size=\footnotesize}

\setbeamerfont{alerted text}{series=\bfseries}
\setbeamerfont{footnote}{size=\scriptsize}
\setbeamerfont{footnote mark}{size=\scriptsize}

\setbeamerfont{caption name}{series=\bfseries}


% .:: Color setup
% ----------------------------------------------------------------------------

\setbeamercolor{itemize item}{fg=TUMBlue, bg=}
\setbeamercolor{itemize subitem}{parent=itemize item}
\setbeamercolor{itemize subsubitem}{parent=itemize item}
\setbeamercolor{enumerate item}{fg=TUMBlue, bg=}
\setbeamercolor{enumerate subitem}{parent=enumerate item}
\setbeamercolor{enumerate subsubitem}{parent=enumerate item}
\setbeamercolor{itemize/enumerate body}{parent=normal text}
\setbeamercolor{itemize/enumerate subbody}{parent=itemize/enumerate body}
\setbeamercolor{itemize/enumerate subsubbody}{parent=itemize/enumerate body}
\setbeamercolor{description item}{fg=TUMBlack, bg=}

\setbeamercolor{item projected}{fg=TUMWhite, bg=TUMBlue}
\setbeamercolor{subitem projected}{parent=item projected}
\setbeamercolor{subsubitem projected}{parent=item projected}
\setbeamercolor{section in toc}{fg=TUMBlack, bg=}
\setbeamercolor{subsection in toc}{parent=section in toc}
\setbeamercolor{subsubsection in toc}{parent=section in toc}

\setbeamercolor{structure}{fg=TUMBlueDark, bg=}
\setbeamercolor{alerted text}{fg=TUMOrange, bg=}
\setbeamercolor{footnote}{fg=TUMBlueDarker, bg=}
\setbeamercolor{footnote mark}{parent=footnote}

\setbeamercolor{caption name}{parent=normal text}

% colors that depend on background are stored in a color theme
\pgfqkeys{/TUM/beamer/colortheme}{
  .is choice,
  % default color theme for white background
  TUM default/.code = {
      \def\@tum@logocolor@{TUMBlue}
      \setbeamercolor{title}{fg=TUMBlue, bg=}
      \setbeamercolor{subtitle}{fg=TUMBlueDark, bg=}
      \setbeamercolor{author}{fg=TUMBlack, bg=}
      \setbeamercolor{institute}{fg=TUMBlack, bg=}
      \setbeamercolor{date}{fg=TUMBlack, bg=}
      \setbeamercolor{frametitle}{fg=TUMBlue, bg=}
      \setbeamercolor{framesubtitle}{fg=TUMBlueDark, bg=}
      \setbeamercolor{headline}{fg=TUMBlue, bg=}
      \setbeamercolor{footline}{fg=TUMBlack, bg=}
      \setbeamercolor{section name}{fg=TUMBlack, bg=}
      \setbeamercolor{section title}{fg=TUMBlue, bg=}
      \setbeamercolor{part name}{fg=TUMBlack, bg=}
      \setbeamercolor{part title}{fg=TUMBlue, bg=}
      \setbeamercolor{normal text}{fg=TUMBlack, bg=}
    },
  % inverse color theme for blue/shaded/flags background
  TUM inverse/.code = {
      \def\@tum@logocolor@{TUMWhite}
      \setbeamercolor{title}{fg=TUMBlueLighter, bg=}
      \setbeamercolor{subtitle}{fg=TUMWhite, bg=}
      \setbeamercolor{author}{fg=TUMWhite, bg=}
      \setbeamercolor{institute}{fg=TUMWhite, bg=}
      \setbeamercolor{date}{fg=TUMWhite, bg=}
      \setbeamercolor{frametitle}{fg=TUMWhite, bg=}
      \setbeamercolor{framesubtitle}{fg=TUMWhite, bg=}
      \setbeamercolor{headline}{fg=TUMWhite, bg=}
      \setbeamercolor{footline}{fg=TUMWhite, bg=}
      \setbeamercolor{section name}{fg=TUMWhite, bg=}
      \setbeamercolor{section title}{fg=TUMBlueLighter, bg=}
      \setbeamercolor{part name}{fg=TUMWhite, bg=}
      \setbeamercolor{part title}{fg=TUMBlueLighter, bg=}
      \setbeamercolor{normal text}{fg=TUMWhite, bg=}
    },
}

% macro to add custom colors to the color theme
\newcommand{\addtoTUMcolortheme}[2]{
  \pgfkeys{/TUM/beamer/colortheme/#1/.append code = {#2}}
}

% load default color theme
\pgfkeys{/TUM/beamer/colortheme=TUM default}


% .:: Custom backgrounds
% ----------------------------------------------------------------------------

\newlength{\@tum@titlewidth@}
\newlength{\@tum@authorwidth@}

\defbeamertemplate{background canvas}{TUM default}{
  \ifbeamercolorempty[bg]{background canvas}{}{%
    \color{bg}\vrule width\paperwidth height\paperheight}
}[action]{
  \setlength{\@tum@titlewidth@}{\textwidth}
  \setlength{\@tum@authorwidth@}{.6\textwidth}
  \pgfqkeys{/TUM/beamer}{
    resources/tower image/.link = /TUM/beamer/resources/tower image blue,
    colortheme = TUM default,
  }
  \usebeamercolor[fg]{normal text}
}

\defbeamertemplate{background canvas}{TUM blue}{
  \pgfuseshading{TUMBlue shading}
}[action]{
  \pgfdeclareverticalshading{TUMBlue shading}{\paperwidth}{
    color(0pt)=(TUMBlue);
    color(\paperheight)=(TUMBlue)
  }
  \setlength{\@tum@titlewidth@}{\textwidth}
  \setlength{\@tum@authorwidth@}{.6\textwidth}
  \pgfqkeys{/TUM/beamer}{
    resources/tower image/.link = /TUM/beamer/resources/tower image white,
    colortheme = TUM inverse,
  }
  \usebeamercolor[fg]{normal text}
}

\defbeamertemplate{background canvas}{TUM shaded}{
  \pgfuseshading{TUMDarker shading}
}[action]{
  \pgfdeclareverticalshading{TUMDarker shading}{\paperwidth}{
    color(0pt)=(TUMBlueDarker);
    color(.15\paperheight)=(TUMBlueDarker);
    color(.85\paperheight)=(TUMBlue);
    color(\paperheight)=(TUMBlue)
  }
  \setlength{\@tum@titlewidth@}{\textwidth}
  \setlength{\@tum@authorwidth@}{.6\textwidth}
  \pgfqkeys{/TUM/beamer}{
    resources/tower image/.link = /TUM/beamer/resources/tower image white,
    colortheme = TUM inverse,
  }
  \usebeamercolor[fg]{normal text}
}

\defbeamertemplate{background canvas}{TUM flags}{
  \pgfuseimage{TUM flag image}
}[action]{
  \if@tum@widescreen@
    \pgfdeclareimage[width=\paperwidth]{TUM flag image}{%
      \pgfkeysvalueof{/TUM/beamer/resources/flag image 169}}
    \setlength{\@tum@titlewidth@}{.62\textwidth}
    \setlength{\@tum@authorwidth@}{.5\textwidth}
  \else
    \pgfdeclareimage[width=\paperwidth]{TUM flag image}{%
      \pgfkeysvalueof{/TUM/beamer/resources/flag image 43}}
    \setlength{\@tum@titlewidth@}{\textwidth}
    \setlength{\@tum@authorwidth@}{.6\textwidth}
  \fi
  \pgfkeys{/TUM/beamer/colortheme = TUM inverse}
  \usebeamercolor[fg]{normal text}
}

\defbeamertemplate{background}{TUM logo}{
  \begin{tikzpicture}[remember picture, overlay]
    \node[anchor=north east, inner sep=0, outer sep=0]
      at (\paperwidth-\@tum@hmargin@, -\@tum@vmargin@)
      {\theTUMLogo[height=\@tum@logoheight@, color=\@tum@logocolor@]};
  \end{tikzpicture}
}

\defbeamertemplate{background}{TUM tower}{
  \begin{tikzpicture}[remember picture, overlay]
    \node[anchor=north east, inner sep=0, outer sep=0]
      at (\paperwidth-\@tum@hmargin@, -\@tum@vmargin@)
      {\theTUMLogo[height=\@tum@logoheight@, color=\@tum@logocolor@]};
    \node[anchor=south east, inner sep=0, outer sep=0]
      at (\paperwidth-\@tum@hmargin@, -\paperheight+\@tum@vmargin@)
      {\pgfuseimage{TUM tower image}};
  \end{tikzpicture}
}[action]{
  \if@tum@widescreen@
    \pgfdeclareimage[width=.5\textwidth]{TUM tower image}{%
      \pgfkeysvalueof{/TUM/beamer/resources/tower image}}
    \setlength{\@tum@titlewidth@}{.8\textwidth}
    \setlength{\@tum@authorwidth@}{.5\textwidth}
  \else
    \pgfdeclareimage[width=.55\textwidth]{TUM tower image}{%
      \pgfkeysvalueof{/TUM/beamer/resources/tower image}}
    \setlength{\@tum@titlewidth@}{\textwidth}
    \setlength{\@tum@authorwidth@}{.55\textwidth}
  \fi
}


% .:: Custom title page
% ----------------------------------------------------------------------------

\newlength{\@tum@frametitleskip@}

\defbeamertemplate{title page}{TUM default}{
  % remove excess vertical space and add precise space
  % (the -2mm are because of \beamer@frametopskip for top alignment)
  \nointerlineskip \vskip-2mm \vskip\@tum@frametitleskip@ \relax
  \begin{beamercolorbox}[wd=\@tum@titlewidth@, ht=2.2cm, dp=0pt, left]{title}
    \vbox to 2.2cm {
      \usebeamerfont{title}\inserttitle\par
      \ifdefempty{\insertsubtitle}{}{
        \medskip
        \usebeamerfont{subtitle}\usebeamercolor[fg]{subtitle}
        \insertsubtitle\par
      }
      \vfil
    }
  \end{beamercolorbox}
  \nointerlineskip
  \bigskip
  \begin{beamercolorbox}[wd=\@tum@authorwidth@, dp=0pt, left]{author}
    \usebeamerfont{author}\insertauthor\par
  \end{beamercolorbox}
  \nointerlineskip
  \bigskip
  \begin{beamercolorbox}[wd=\@tum@authorwidth@, dp=0pt, left]{institute}
    \usebeamerfont{institute}\insertinstitute\par
  \end{beamercolorbox}
  \nointerlineskip
  \bigskip
  \begin{beamercolorbox}[wd=\@tum@authorwidth@, dp=0pt, left]{date}
    \usebeamerfont{date}\insertdate\par
  \end{beamercolorbox}
}

\defbeamertemplate{title page}{TUM centered}{
  \vfill
  \begin{beamercolorbox}[wd=\paperwidth, ht=2.2cm, dp=0pt, center]{title}
    \vbox to 2.2cm {
      \usebeamerfont{title}\inserttitle\par
      \ifdefempty{\insertsubtitle}{}{
        \medskip
        \usebeamerfont{subtitle}\usebeamercolor[fg]{subtitle}
        \insertsubtitle\par
      }
      \vfil
    }
  \end{beamercolorbox}
  \nointerlineskip
  \bigskip
  \begin{beamercolorbox}[wd=\paperwidth, dp=0pt, center]{author}
    \usebeamerfont{author}\insertauthor\par
  \end{beamercolorbox}
  \nointerlineskip
  \bigskip
  \begin{beamercolorbox}[wd=\paperwidth, dp=0pt, center]{institute}
    \usebeamerfont{institute}\insertinstitute\par
  \end{beamercolorbox}
  \nointerlineskip
  \bigskip
  \begin{beamercolorbox}[wd=\paperwidth, dp=0pt, center]{date}
    \usebeamerfont{date}\insertdate\par
  \end{beamercolorbox}
}

% redefine maketitle command to load selected layout
\renewcommand{\maketitle}{
  \TUMframestyle{title page}
  \begin{frame}[t]
    \titlepage
  \end{frame}
  \TUMframestyle{content page}
}


% .:: Custom header and footer
% ----------------------------------------------------------------------------

\newlength{\@tum@headheight@}
\newlength{\@tum@footheight@}
\newlength{\@tum@footdepth@}
\setlength{\@tum@footdepth@}{1.5mm}

\defbeamertemplate{headline}{TUM empty}{
}[action]{
  \setlength{\@tum@headheight@}{0pt}
  \setlength{\@tum@frametitleskip@}{2\@tum@vmargin@ + \@tum@logoheight@}
}

\defbeamertemplate{headline}{TUM oneliner}{
  \begin{beamercolorbox}[ht=\@tum@headheight@, dp=0pt,
      leftskip=\@tum@hmargin@, rightskip=\@tum@hmargin@]{headline}
    % use parbox with three lines of text and resize it to the logoheight
    \resizebox{!}{\@tum@logoheight@}{\parbox[b]{\textwidth}{
        \fontfamily{phv}\selectfont\footnotesize\usebeamercolor[fg]{headline}
        \phantom{University}\newline\phantom{University}\newline
        \theUniversityName\phantom{University}
      }}
  \end{beamercolorbox}
}[action]{
  \setlength{\@tum@headheight@}{\@tum@vmargin@ + \@tum@logoheight@}
  \setlength{\@tum@frametitleskip@}{\@tum@headheight@ + \@tum@vmargin@}
}

\defbeamertemplate{headline}{TUM twoliner}{
  \begin{beamercolorbox}[ht=\@tum@headheight@, dp=0pt,
      leftskip=\@tum@hmargin@, rightskip=\@tum@hmargin@]{headline}
    % use parbox with three lines of text and resize it to the logoheight
    \resizebox{!}{\@tum@logoheight@}{\parbox[b]{\textwidth}{
        \fontfamily{phv}\selectfont\footnotesize\usebeamercolor[fg]{headline}
        \phantom{University}\newline
        \rlap{\theDepartmentName}{\phantom{University}}\newline
        \theUniversityName\phantom{University}
      }}
  \end{beamercolorbox}
}[action]{
  \setlength{\@tum@headheight@}{\@tum@vmargin@ + \@tum@logoheight@}
  \setlength{\@tum@frametitleskip@}{\@tum@headheight@ + \@tum@vmargin@}
}

\defbeamertemplate{headline}{TUM threeliner}{
  \begin{beamercolorbox}[ht=\@tum@headheight@, dp=0pt,
      leftskip=\@tum@hmargin@, rightskip=\@tum@hmargin@]{headline}
    % use parbox with three lines of text and resize it to the logoheight
    \resizebox{!}{\@tum@logoheight@}{\parbox[b]{\textwidth}{
        % specify font and color for the header
        \fontfamily{phv}\selectfont\footnotesize\usebeamercolor[fg]{headline}
        % use phantoms here to ensure every line has the same height and depth
        % (important if e.g. chair is empty)
        \leavevmode\rlap{\theChairName}{\phantom{University}}\newline
        \rlap{\theDepartmentName}{\phantom{University}}\newline
        \theUniversityName\phantom{University}
      }}
  \end{beamercolorbox}
}[action]{
  \setlength{\@tum@headheight@}{\@tum@vmargin@ + \@tum@logoheight@}
  \setlength{\@tum@frametitleskip@}{\@tum@headheight@ + \@tum@vmargin@}
}

\defbeamertemplate{headline}{TUM logothreeliner}{
  \begin{beamercolorbox}[ht=\@tum@headheight@, dp=0pt,
      leftskip=\@tum@hmargin@, rightskip=\@tum@hmargin@]{headline}
    \ifdef{\theDepartmentLogo}{%
      \theDepartmentLogo[height=\@tum@logoheight@, color=\@tum@logocolor@]%
      \hspace{.5\@tum@logoheight@}%
    }{%
      \ClassWarning{tumbeamer}{Department logo is not defined!}
    }%
    % use parbox with three lines of text and resize it to the logoheight
    \resizebox{!}{\@tum@logoheight@}{\parbox[b]{\textwidth}{
        \fontfamily{phv}\selectfont\footnotesize\usebeamercolor[fg]{headline}
        \leavevmode\rlap{\theChairName}{\phantom{University}}\newline
        \rlap{\theDepartmentName}{\phantom{University}}\newline
        \theUniversityName\phantom{University}
      }}
  \end{beamercolorbox}
}[action]{
  \setlength{\@tum@headheight@}{\@tum@vmargin@ + \@tum@logoheight@}
  \setlength{\@tum@frametitleskip@}{\@tum@headheight@ + \@tum@vmargin@}
}

\defbeamertemplate{footline}{TUM empty}{
  \begin{beamercolorbox}[ht=\@tum@footheight@, dp=\@tum@footdepth@,
      leftskip=\@tum@hmargin@, rightskip=\@tum@hmargin@, ignorebg]{footline}
  \end{beamercolorbox}
}[action]{
  \setlength{\@tum@footheight@}{\@tum@vmargin@ - \@tum@footdepth@}
}

\defbeamertemplate{footline}{TUM default}{
  \begin{beamercolorbox}[ht=\@tum@footheight@, dp=\@tum@footdepth@,
      leftskip=\@tum@hmargin@, rightskip=\@tum@hmargin@]{footline}
    \insertfootline\hfill\insertframenumber
  \end{beamercolorbox}
}[action]{
  \setlength{\@tum@footheight@}{\@tum@vmargin@ - \@tum@footdepth@}
}

\defbeamertemplate{footline}{TUM infoline}{
  \begin{beamercolorbox}[ht=\@tum@footheight@, dp=\@tum@footdepth@,
      leftskip=\@tum@hmargin@, rightskip=\@tum@hmargin@]{footline}
    \insertshortauthor\hfill
    \insertshorttitle\hfill
    \insertshortdate\hfill
    Page~\insertframenumber/\inserttotalframenumber
  \end{beamercolorbox}
}[action]{
  \setlength{\@tum@footheight@}{\@tum@vmargin@ - \@tum@footdepth@}
}

% define custom footline text
\newcommand{\insertfootline}{\strut}
\newcommand{\footline}[1]{\renewcommand{\insertfootline}{#1}}

% make sure that page layout is calculated with empty head and foot
\AfterEndPreamble{
  \setbeamertemplate{headline}[TUM empty]
  \setbeamertemplate{footline}[TUM empty]
}


% .:: Custom frame title
% ----------------------------------------------------------------------------

\defbeamertemplate{frametitle}{TUM default}{
  % remove excess vertical space and add precise space
  \nointerlineskip \vskip\@tum@frametitleskip@ \relax
  \begin{beamercolorbox}[wd=\paperwidth, dp=0mm,
      leftskip=\@tum@hmargin@, rightskip=20mm]{frametitle}
    {\usebeamerfont{frametitle}\insertframetitle\par}
  \end{beamercolorbox}
  \ifdefempty{\insertframesubtitle}{}{
    \usebeamerfont{framesubtitle}\usebeamercolor[fg]{framesubtitle}
    \insertframesubtitle\par
  }
}

\defbeamertemplate{frametitle}{TUM more space}{
  % remove excess vertical space and add precise space
  \nointerlineskip \vskip\@tum@vmargin@ \relax
  \begin{beamercolorbox}[wd=\paperwidth, ht=\@tum@logoheight@, dp=0mm,
      leftskip=\@tum@hmargin@, rightskip=20mm]{frametitle}
    {\usebeamerfont{frametitle}\insertframetitle\par}
  \end{beamercolorbox}
  \ifdefempty{\insertframesubtitle}{}{
    \usebeamerfont{framesubtitle}\usebeamercolor[fg]{framesubtitle}
    \insertframesubtitle\par
  }
}

% Changed definition from 'beamerbaseframe.sty'
% to modify spacing after frame title
\define@key{beamerframe}{c}[true]{
  \beamer@frametopskip=0pt plus 1fill\relax
  \beamer@framebottomskip=0pt plus 1.5fill\relax
  \beamer@frametopskipautobreak=0pt plus .4\paperheight\relax
  \beamer@framebottomskipautobreak=0pt plus .6\paperheight\relax
}
\define@key{beamerframe}{t}[true]{
  \beamer@frametopskip=2mm\relax  %  <-- I've set this to 2mm!
  \beamer@framebottomskip=0pt plus 1fill\relax
  \beamer@frametopskipautobreak=\beamer@frametopskip\relax
  \beamer@framebottomskipautobreak=\beamer@framebottomskip\relax
}
\define@key{beamerframe}{b}[true]{
  \beamer@frametopskip=0pt plus 1fill\relax
  \beamer@framebottomskip=0pt\relax
  \beamer@frametopskipautobreak=\beamer@frametopskip\relax
  \beamer@framebottomskipautobreak=\beamer@framebottomskip\relax
}
\def\beamer@initfirstlineunskip{}  %  <-- compatibility with texlive 2016


% .:: Custom part pages
% ----------------------------------------------------------------------------

\defbeamertemplate{part page}{TUM toc}{
  \frametitle{\partname~\insertromanpartnumber:\;\insertpart}
  \tableofcontents
}

\defbeamertemplate{part page}{TUM default}{
  \nointerlineskip
  \begin{beamercolorbox}[wd=\@tum@authorwidth@, dp=0mm]{part title}
    {\usebeamerfont{part name}\usebeamercolor[fg]{part name}
      \partname~\insertromanpartnumber\par}
    \vspace{5mm}
    {\usebeamerfont{part title}\insertpart\par}
  \end{beamercolorbox}
}

\defbeamertemplate{part page}{TUM centered}{
  \nointerlineskip
  \begin{beamercolorbox}[wd=\paperwidth, dp=0mm, center]{part title}
    {\usebeamerfont{part name}\usebeamercolor[fg]{part name}
      \partname~\insertromanpartnumber\par}
    \vspace{5mm}
    {\usebeamerfont{part title}\insertpart\par}
  \end{beamercolorbox}
}

% enable or disable part pages
\if@tum@partpage@
  \AtBeginPart{
    \TUMframestyle{part page}
    \begin{frame}[c, noframenumbering]
      \partpage
    \end{frame}
    \TUMframestyle{content page}
  }
\else
  \AtBeginPart{}
\fi


% .:: Custom section pages
% ----------------------------------------------------------------------------

\defbeamertemplate{section page}{TUM toc}{
  \frametitle{Outline}
  \tableofcontents[currentsection, hideothersubsections]
}

\defbeamertemplate{section page}{TUM default}{
  \nointerlineskip
  \begin{beamercolorbox}[wd=\@tum@authorwidth@, dp=0mm, left]{section title}
    {\usebeamerfont{section title}\insertsection\par}
  \end{beamercolorbox}
}

\defbeamertemplate{section page}{TUM centered}{
  \nointerlineskip
  \begin{beamercolorbox}[wd=\paperwidth, dp=0mm, center]{section title}
    {\usebeamerfont{section title}\insertsection\par}
  \end{beamercolorbox}
}

% enable or disable section pages
\if@tum@sectionpage@
  \AtBeginSection[]{  % do nothing for \section*
    \TUMframestyle{section page}
    \begin{frame}[c, noframenumbering]
      \sectionpage
    \end{frame}
    \TUMframestyle{content page}
  }
\else
  \AtBeginSection[]{}
\fi


% .:: Custom ToC
% ----------------------------------------------------------------------------

\newlength{\@tum@tocsecsymbol@}
\newlength{\@tum@tocsecnumber@}
\newlength{\@tum@tocsubsecsymbol@}

\defbeamertemplate*{section in toc}{TUM default}{
  \leavevmode\rlap{%
    \usebeamercolor[bg]{section number projected}%
    \rule{\@tum@tocsecsymbol@}{\@tum@tocsecsymbol@}%
  }{%
    \usebeamerfont{section number projected}%
    \usebeamercolor[fg]{section number projected}%
    \raisebox{.5\@tum@tocsecsymbol@-.5\@tum@tocsecnumber@}{%
      \hbox to \@tum@tocsecsymbol@{\hfil\inserttocsectionnumber\hfil}}%
  }%
  \hspace{\@tum@tocsecsymbol@}%
  {\usebeamerfont{section in toc}\usebeamercolor[fg]{section in toc}%
    \inserttocsection\par}
}[action]{
  \settoheight{\@tum@tocsecsymbol@}{
    \usebeamerfont{section in toc}H}
  \settoheight{\@tum@tocsecnumber@}{
    \usebeamerfont{section number projected}1}
}

\defbeamertemplate*{subsection in toc}{TUM default}{
  \hspace{2\@tum@tocsecsymbol@}%
  \usebeamercolor[bg]{section number projected}%
  \leavevmode\hbox to \@tum@tocsubsecsymbol@{\hfil%
    \raisebox{.15\@tum@tocsubsecsymbol@}{%
      \rule{.7\@tum@tocsubsecsymbol@}{.7\@tum@tocsubsecsymbol@}}\hfil}%
  \hspace{\@tum@tocsubsecsymbol@}%
  {\usebeamerfont{subsection in toc}\usebeamercolor[fg]{subsection in toc}%
    \inserttocsubsection\par}
}[action]{
  \settoheight{\@tum@tocsubsecsymbol@}{
    \usebeamerfont{subsection in toc}H}
}

\defbeamertemplate*{subsubsection in toc}{TUM default}{
  \leavevmode\hspace{2\@tum@tocsecsymbol@}%
  \hspace{2\@tum@tocsubsecsymbol@}%
  {\usebeamerfont{subsubsection in toc}%
    \usebeamercolor{subsubsection in toc}%
    \inserttocsubsubsection\par}
}


% .:: Custom itemize, enumerate, and description
% ----------------------------------------------------------------------------

\setbeamertemplate{itemize item}{$\blacksquare$}
\setbeamertemplate{itemize subitem}{$\square$}
\setbeamertemplate{itemize subsubitem}{$-$}

\setbeamertemplate{enumerate item}{\arabic{enumi}.}
\setbeamertemplate{enumerate subitem}{(\alph{enumii})}
\setbeamertemplate{enumerate subsubitem}{(\roman{enumii})}

% Changed definitions from 'beamerbaselocalstructure.sty' for list spacing
\setlength{\leftmargini}{6mm}
\setlength{\leftmarginii}{6mm}
\setlength{\leftmarginiii}{6mm}
\setlength{\labelsep}{2mm}
\setlength{\labelwidth}{5mm}
\setlength{\beamer@descriptionwidth}{15mm}
\def\@listi{\leftmargin\leftmargini
  \topsep  0\p@ \@plus 0\p@ \@minus 0\p@
  \parsep  0\p@
  \itemsep 3\p@ \@plus 1\p@ \@minus 1\p@}
\let\@listI\@listi
\def\@listii{\leftmargin\leftmarginii
  \topsep  3\p@ \@plus 0\p@ \@minus 0\p@
  \parsep  0\p@
  \itemsep 2\p@ \@plus 1\p@ \@minus 1\p@}
\def\@listiii{\leftmargin\leftmarginiii
  \topsep  0\p@ \@plus 0\p@ \@minus 0\p@
  \parsep  0\p@
  \itemsep 0\p@ \@plus 0\p@ \@minus 0\p@}


% .:: Frame style manipulation
% ----------------------------------------------------------------------------

% user command to load a particular frame style
\newcommand{\TUMframestyle}[1]{
  \pgfkeysifassignable{/TUM/beamer/framestyle/#1}{
    \pgfkeys{/TUM/beamer/framestyle/#1}
  }{
    \ClassError{tumbeamer}{I can't use framestyle '#1',
      because it doesn't exist}{Use '\textbackslash defTUMframestyle'
      first to define a framestyle}
  }
}

% user command to define a frame style
\newcommand{\defTUMframestyle}[2]{
  \pgfkeysifassignable{/TUM/beamer/framestyle/#1}{
    \ClassError{tumbeamer}{I can't define framestyle '#1',
      because it already exists}{Use '\textbackslash setTUMframestyle'
      to redefine an existing framestyle}
  }{
    \pgfkeys{/TUM/beamer/framestyle/#1/.code = #2}
  }
}

% user command to set an existing frame style
\newcommand{\setTUMframestyle}[2]{
  \pgfkeysifassignable{/TUM/beamer/framestyle/#1}{
    \pgfkeys{/TUM/beamer/framestyle/#1/.code = #2}
  }{
    \ClassError{tumbeamer}{I can't set framestyle '#1',
      because it doesn't exist}{Use '\textbackslash defTUMframestyle'
      first to define a framestyle}
  }
}

% user command to add code to an existing frame style
\newcommand{\addtoTUMframestyle}[2]{
  \pgfkeysifassignable{/TUM/beamer/framestyle/#1}{
    \pgfkeys{/TUM/beamer/framestyle/#1/.append code = #2}
  }{
    \ClassError{tumbeamer}{I can't add to framestyle '#1',
      because it doesn't exist}{Use '\textbackslash defTUMframestyle'
      first to define a framestyle}
  }
}

% keys to store frame styles
\pgfqkeys{/TUM/beamer/framestyle}{
  title page/.code   = {},
  part page/.code    = {},
  section page/.code = {},
  content page/.code = {},
}


% .:: Predefined frame styles
% ----------------------------------------------------------------------------

% keys that store title page options
\pgfqkeys{/TUM/beamer/setup/title page}{
  .is choice,
  TUM default/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{title page}[TUM default]
      }
    },
  TUM centered/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{title page}[TUM centered]
      }
    },
  TUM tower/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{title page}[TUM default]
      }
    },
  TUM blue default/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{title page}[TUM default]
      }
    },
  TUM blue centered/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{title page}[TUM centered]
      }
    },
  TUM blue tower/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{title page}[TUM default]
      }
    },
  TUM shaded default/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{title page}[TUM default]
      }
    },
  TUM shaded centered/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{title page}[TUM centered]
      }
    },
  TUM shaded tower/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{title page}[TUM default]
      }
    },
  TUM flags/.code = {
      \setTUMframestyle{title page}{
        \setbeamertemplate{background canvas}[TUM flags]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{title page}[TUM default]
      }
    },
  TUM blue/.forward to = {
      /TUM/beamer/setup/title page/TUM blue default},
  TUM shaded/.forward to = {
      /TUM/beamer/setup/title page/TUM shaded default},
}

% keys that store part page options
\pgfqkeys{/TUM/beamer/setup/part page}{
  .is choice,
  TUM default/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{part page}[TUM default]
      }
    },
  TUM centered/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{part page}[TUM centered]
      }
    },
  TUM tower/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{part page}[TUM default]
      }
    },
  TUM toc/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{part page}[TUM toc]
      }
    },
  TUM blue default/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{part page}[TUM default]
      }
    },
  TUM blue centered/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{part page}[TUM centered]
      }
    },
  TUM blue tower/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{part page}[TUM default]
      }
    },
  TUM shaded default/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{part page}[TUM default]
      }
    },
  TUM shaded centered/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{part page}[TUM centered]
      }
    },
  TUM shaded tower/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{part page}[TUM default]
      }
    },
  TUM flags/.code = {
      \setTUMframestyle{part page}{
        \setbeamertemplate{background canvas}[TUM flags]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{part page}[TUM default]
      }
    },
  TUM blue/.forward to = {
      /TUM/beamer/setup/part page/TUM blue default},
  TUM shaded/.forward to = {
      /TUM/beamer/setup/part page/TUM shaded default},
}

% keys that store section page options
\pgfqkeys{/TUM/beamer/setup/section page}{
  .is choice,
  TUM default/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{section page}[TUM default]
      }
    },
  TUM centered/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{section page}[TUM centered]
      }
    },
  TUM tower/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{section page}[TUM default]
      }
    },
  TUM toc/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{section page}[TUM toc]
      }
    },
  TUM blue default/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{section page}[TUM default]
      }
    },
  TUM blue centered/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{section page}[TUM centered]
      }
    },
  TUM blue tower/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{section page}[TUM default]
      }
    },
  TUM shaded default/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{section page}[TUM default]
      }
    },
  TUM shaded centered/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{section page}[TUM centered]
      }
    },
  TUM shaded tower/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM tower]
        \setbeamertemplate{section page}[TUM default]
      }
    },
  TUM flags/.code = {
      \setTUMframestyle{section page}{
        \setbeamertemplate{background canvas}[TUM flags]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{section page}[TUM default]
      }
    },
  TUM blue/.forward to = {
      /TUM/beamer/setup/section page/TUM blue default},
  TUM shaded/.forward to = {
      /TUM/beamer/setup/section page/TUM shaded default},
}

% keys that store content page options
\pgfqkeys{/TUM/beamer/setup/content page}{
  .is choice,
  TUM default/.code = {
      \setTUMframestyle{content page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{frametitle}[TUM default]
      }
    },
  TUM more space/.code = {
      \setTUMframestyle{content page}{
        \setbeamertemplate{background canvas}[TUM default]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{frametitle}[TUM more space]
      }
    },
  TUM blue/.code = {
      \setTUMframestyle{content page}{
        \setbeamertemplate{background canvas}[TUM blue]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{frametitle}[TUM default]
      }
    },
  TUM shaded/.code = {
      \setTUMframestyle{content page}{
        \setbeamertemplate{background canvas}[TUM shaded]
        \setbeamertemplate{background}[TUM logo]
        \setbeamertemplate{frametitle}[TUM default]
      }
    },
}

% TeX ifs to switch headlines and footlines on and off on different pages
\newif\if@tum@beamer@headline@titlepage@
\newif\if@tum@beamer@headline@partpage@
\newif\if@tum@beamer@headline@sectionpage@
\newif\if@tum@beamer@headline@contentpage@
\newif\if@tum@beamer@footline@titlepage@
\newif\if@tum@beamer@footline@partpage@
\newif\if@tum@beamer@footline@sectionpage@
\newif\if@tum@beamer@footline@contentpage@

% keys for headline and footline options
\pgfqkeys{/TUM/beamer/setup}{
  headline/.initial = TUM empty,
  footline/.initial = TUM empty,
  headline on/title page/.is if   = @tum@beamer@headline@titlepage@,
  headline on/part page/.is if    = @tum@beamer@headline@partpage@,
  headline on/section page/.is if = @tum@beamer@headline@sectionpage@,
  headline on/content page/.is if = @tum@beamer@headline@contentpage@,
  headline on/every page/.style   = {title page, part page,
      section page, content page},
  footline on/title page/.is if   = @tum@beamer@footline@titlepage@,
  footline on/part page/.is if    = @tum@beamer@footline@partpage@,
  footline on/section page/.is if = @tum@beamer@footline@sectionpage@,
  footline on/content page/.is if = @tum@beamer@footline@contentpage@,
  footline on/every page/.style   = {title page, part page,
      section page, content page},
  headline on/.code = {\pgfqkeys{/TUM/beamer/setup/headline on}{#1}},
  footline on/.code = {\pgfqkeys{/TUM/beamer/setup/footline on}{#1}},
}

% macro to define TUMbeamer layout with key-value options
\newcommand{\TUMbeamersetup}[1]{
  % process key-value options
  \pgfqkeys{/TUM/beamer/setup}{#1}
  % set headlines
  \if@tum@beamer@headline@titlepage@
    \addtoTUMframestyle{title page}{\setbeamertemplate{headline}[%
        \pgfkeysvalueof{/TUM/beamer/setup/headline}]}
  \else
    \addtoTUMframestyle{title page}{\setbeamertemplate{headline}[%
        TUM empty]}
  \fi
  \if@tum@beamer@headline@partpage@
    \addtoTUMframestyle{part page}{\setbeamertemplate{headline}[%
        \pgfkeysvalueof{/TUM/beamer/setup/headline}]}
  \else
    \addtoTUMframestyle{part page}{\setbeamertemplate{headline}[%
        TUM empty]}
  \fi
  \if@tum@beamer@headline@sectionpage@
    \addtoTUMframestyle{section page}{\setbeamertemplate{headline}[%
        \pgfkeysvalueof{/TUM/beamer/setup/headline}]}
  \else
    \addtoTUMframestyle{section page}{\setbeamertemplate{headline}[%
        TUM empty]}
  \fi
  \if@tum@beamer@headline@contentpage@
    \addtoTUMframestyle{content page}{\setbeamertemplate{headline}[%
        \pgfkeysvalueof{/TUM/beamer/setup/headline}]}
  \else
    \addtoTUMframestyle{content page}{\setbeamertemplate{headline}[%
        TUM empty]}
  \fi
  % set footlines
  \if@tum@beamer@footline@titlepage@
    \addtoTUMframestyle{title page}{\setbeamertemplate{footline}[%
        \pgfkeysvalueof{/TUM/beamer/setup/footline}]}
  \else
    \addtoTUMframestyle{title page}{\setbeamertemplate{footline}[%
        TUM empty]}
  \fi
  \if@tum@beamer@footline@partpage@
    \addtoTUMframestyle{part page}{\setbeamertemplate{footline}[%
        \pgfkeysvalueof{/TUM/beamer/setup/footline}]}
  \else
    \addtoTUMframestyle{part page}{\setbeamertemplate{footline}[%
        TUM empty]}
  \fi
  \if@tum@beamer@footline@sectionpage@
    \addtoTUMframestyle{section page}{\setbeamertemplate{footline}[%
        \pgfkeysvalueof{/TUM/beamer/setup/footline}]}
  \else
    \addtoTUMframestyle{section page}{\setbeamertemplate{footline}[%
        TUM empty]}
  \fi
  \if@tum@beamer@footline@contentpage@
    \addtoTUMframestyle{content page}{\setbeamertemplate{footline}[%
        \pgfkeysvalueof{/TUM/beamer/setup/footline}]}
  \else
    \addtoTUMframestyle{content page}{\setbeamertemplate{footline}[%
        TUM empty]}
  \fi
}

% process standard setup
\TUMbeamersetup{
  title page   = TUM tower,
  part page    = TUM toc,
  section page = TUM toc,
  content page = TUM default,
  headline     = TUM threeliner,
  headline on  = {title page},
  footline     = TUM default,
  footline on  = {every page, title page=false},
}

% load content page style at begin of document
\AtBeginDocument{
  \TUMframestyle{content page}
}
