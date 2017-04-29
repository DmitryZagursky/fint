%This script is done to perform analysis of the latest measurements and to build some graphics with that.

page_screen_output(0);
page_output_immediately(1);%octave printing to console settings

disp("Getting data");
d=get_data('results/Drugie_500.txt');
air=d(:,1:10); tim=d(:,11:20); div=d(:,21:30);
disp("Counting correlations");
cor=matr_corr(d);
pcolor(cor);
grid;
title("Correlations between the various measurements");
axis("square");
colorbar;
