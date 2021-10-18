%seperate traj

% traj{1,max(data(:,1))}={};
% count=1;
% for i=1:length(data)-1
%     if data(i,1)==count
%         traj{1,count}(end+1,:)=data(i,:);
%     end
%     if data(i+1,1)==count+1
%         count=count+1;
%     end
% end

%%
count=1;
record=[1];
for i=1:length(traj)
    inil=traj{1,i}(1,13);
    for k=1:length(traj{1,i})
        curl=traj{1,i}(k,13);
        if curl~=inil
            record=[record,k];
            inil=curl;
        end
    end
    if length(record)==1
        figure(inil+2);hold all
        plot(traj{1,i}(:,3),traj{1,i}(:,5));
        text(traj{1,i}(record(1),3),traj{1,i}(record(1),5),num2str(i));
    else
        record=[record,length(traj{1,i})+1];
        for j=1:length(record)-1
            if traj{1,i}(record(1,j),13)==-1
                figure(traj{1,i}(record(j),13)+2);hold all
                plot(traj{1,i}(record(j):record(j+1)-1,3),traj{1,i}(record(j):record(j+1)-1,5));
                text(traj{1,i}(record(j),3),traj{1,i}(record(j),5),num2str(i));
            end
            if traj{1,i}(record(1,j),13)==0
                figure(traj{1,i}(record(j),13)+2);hold all
                plot(traj{1,i}(record(j):record(j+1)-1,3),traj{1,i}(record(j):record(j+1)-1,5));
                text(traj{1,i}(record(j),3),traj{1,i}(record(j),5),num2str(i));
            end
            if traj{1,i}(record(1,j),13)==1
                figure(traj{1,i}(record(j),13)+2);hold all
                plot(traj{1,i}(record(j):record(j+1)-1,3),traj{1,i}(record(j):record(j+1)-1,5));
                text(traj{1,i}(record(j),3),traj{1,i}(record(j),5),num2str(i));
            end
            if traj{1,i}(record(1,j),13)==2
                figure(traj{1,i}(record(j),13)+2);hold all
                plot(traj{1,i}(record(j):record(j+1)-1,3),traj{1,i}(record(j):record(j+1)-1,5));
                text(traj{1,i}(record(j),3),traj{1,i}(record(j),5),num2str(i));
            end
        end             
    end
    record=[1];
end

figure(1);
title('Ramp');
figure(2);
title('Lane 1');
figure(3);
title('Lane 2');
figure(4);
title('Lane 3');