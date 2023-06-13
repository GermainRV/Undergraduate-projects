clear
tic
%% Datos de entrada:
    % Intervalo de tiempo [a,b]
    a = 0; % (s)
    b = 20; % (s)
    % Número de masas
    N = 5;
    % Constante del resorte k, masa total M, longitud total L
    k =1; % (N/m)
    M = 1; % (kg)
    L = 1; % (m)
    % Incremento
    h=0.1; % (s)
    
%% Parámetros auxiliares
n = 2*N; % Número de EDOs de primer orden
iter = (b-a)/h; % Número de iteraciones
m = M/N; % Masa individual
L0 = L/(N-1); % Longitud individual de cada resorte

%% Definiendo las funciones f
syms 'y%d' [N 2]
g(:,1) = y(:,2);
g(N+1,1) = (k/m).*(y21 - y11);%-(k/m)*y11;% Término para extremo inicial fijo
for i = 2:N-1
    g(N+i,1) = (k/m).*(y(i-1,1) - 2*y(i,1) + y(i+1,1));
end
g(n,1) = (k/m).*(y(N-1,1) - y(N,1));%-(k/m)*y(N,1);% Término para extremo final fijo

f = cell(n,1);
for i=1:n
    f{i} = symfun(g(i),[y(:,1); y(:,2)]);
end
varY = [y(:,1); y(:,2)]; % Contiene todas las variables y_ij
clearvars y*

%% Asignando condiciones iniciales
    % Inicializando la matriz t e y
    t = zeros(iter,1);
    w = zeros(n,iter);
    % Instante inicial
    t(1) = a;
    
    % Condiciones iniciales w(0) = alpha, w'(0) = beta
    alpha(:,1) = -L0/2 + L0*rand(N,1); % Deformación aleatoria


%     if(mod(N,2)==0) % Condicion de traccion en ambos extremos
%         alpha(1:N/2,1) = -L0/2;
%         alpha(N/2+1:N,1) = L0/2;
%     else
%         alpha(1:(N-1)/2,1) = -L0/2;
%         alpha((N+1)/2,1) = 0;
%         alpha((N+1)/2+1:N,1) = L0/2;
%     end
    
    if(mod(N,2)==0) % Condicion de compresion en ambos extremos
        alpha(1:N/2,1) = L0/2;
        alpha(N/2:N,1) = -L0/2;
    else
        alpha(1:(N-1)/2,1) = L0/2;
        alpha((N+1)/2,1) = 0;
        alpha((N+1)/2+1:N,1) = -L0/2;
    end


    beta(:,1) = zeros(N,1); % Comienza del reposo
    for i = 1:N
        w(i,1) = alpha(i);
        w(i+N,1) = beta(i); 
    end
%% Implementación del algoritmo de Runge-Kutta orden 4
k1 = zeros(n,1);
k2 = zeros(n,1);
k3 = zeros(n,1);
k4 = zeros(n,1);
for j=1:iter
    for i=1:n
        k1(i,1) = h*subs(f{i},varY,w(:,j));
    end
    for i=1:n
        k2(i,1) = h*subs(f{i},varY,w(:,j)+k1(:)/2);
    end
    for i=1:n
        k3(i,1) = h*subs(f{i},varY,w(:,j)+k2(:)/2);
    end
    for i=1:n
        k4(i,1) = h*subs(f{i},varY,w(:,j)+k3(:));
    end
    w(:,j+1) = w(:,j) + (1/6)*(k1(:) + 2*k2(:) + 2*k3(:) + k4(:));
    t(j+1) = t(1) + j*h; 
end
w = w';

%% Transformación de la deformación w a coordenada posición z
z = zeros(size(w,1),size(w,2)/2); % Posición de cada masa
Vz = zeros(size(w,1),size(w,2)/2); % Velocidad de cada masa
for i=1:size(w,1)
    for j=1:N
        z(i,j) = (j-1)*L0 + w(i,j);
        Vz(i,j) = w(i,N+j);
    end
end
%% Visualización mediante gráficas
figure(1)
for i=1:N
    plot([(i-1)*L0 (i-1)*L0],[a b],'k--') % Posicion de equilibrio de las
    %masas, no es valido para condiciones iniciales aleatorias
    hold on
    plot(z(:,i),t)    
end
hold off
% xlim([-0.05 1.2])
% ylim([0.0 20.0])
xlabel("Posici\'{o}n de la masa $m_i$ (m)",'Interpreter',"latex");
ylabel('Tiempo (s)','Interpreter',"latex");
title(['Oscilaciones de un sistema de ',num2str(N),' masas acopladas por resortes'])
toc
