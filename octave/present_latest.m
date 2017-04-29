%This script is done to perform analysis of the latest measurements and to build some graphics with that.

page_screen_output(0);
page_output_immediately(1);%octave printing to console settings

disp("Getting data, Dima");
d=get_data('results/Drugie_500.txt');
%air=d(:,1:10); tim=d(:,11:20); div=d(:,21:30);
disp("Counting correlations");
cor=matr_corr(d);
pcolor(cor);%Drawing and customizing plot
grid;
title("Correlations between the various measurements, Dima");
axis("square");
colorbar;

disp("Getting data, Nikita");
d=get_data('results/500.txt');
%air=d(:,1:10); tim=d(:,11:20); div=d(:,21:30);
disp("Counting correlations");
cor=matr_corr(d);
figure;
pcolor(cor);
grid;
title("Correlations between the various measurements, Nikita");
axis("square");
colorbar;

