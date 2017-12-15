#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>


#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include <math.h>

int GORY = 0;
bool boommax = false;
bool textureMode = false;
bool lightMode = false;
double xb;
double yb;
double zb;
double SCHETCHIK = 0;
bool alpha = false;
double XV = 0, YV = 0, ZV = 0;
bool osnova = true;
bool osnovaSomolet = true;
double Vector[3];
double razmer = 0.1;
double Rgrib = 0.1;
bool est_popodanie = false;
int rendCount = 0;
double t1 = 0.1;
double t = 0;
int direction = 1;
double PointPrev[3] = { 0 };
double PointCur[3] = { 0 };
double dir[] = { 1,1,1 };
double prevVec[3] = { 0 };
bool flagStart = true;
double PointEnd[3] = { 0 };
double PointBegin[3] = { 0 };
double BEZIE = 1;
bool flag_alpha = false;
bool flag_Bomb_Down = false;
double ugolP = 0;
double masbezieX[16];
double masbezieY[16];
bool popolnenie = true;

GLuint texId[6];

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;


	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой

		lookPoint.setCoords(XV, YV, 10.5*razmer);

		/*pos.setCoords(camDist*cos(fi2)*cos(fi1),
		camDist*cos(fi2)*sin(fi1),
		camDist*sin(fi2));*/
		if (!flag_Bomb_Down)
		{
			camDist = 5;
			pos.setCoords(XV + camDist*cos(fi2)*cos(fi1),
				YV + camDist*cos(fi2)*sin(fi1),
				(10.5*razmer) + camDist*sin(fi2));
		}
		else
			if (!est_popodanie && razmer<1)
			{
				lookPoint.setCoords(XV, YV, 0);
				camDist = 1;
				pos.setCoords(PointEnd[0] + camDist*cos(fi2)*cos(fi1),
					PointEnd[1] + camDist*cos(fi2)*sin(fi1),
					PointEnd[2] + camDist*sin(fi2));
			}
			else
				if (!boommax || razmer<1)
				{
					if (camDist<63)
						camDist += 1;
					fi2 = 0;
					pos.setCoords(XV + camDist*cos(fi2)*cos(fi1),
						YV + camDist*cos(fi2)*sin(fi1),
						(10 * Rgrib) + camDist*sin(fi2));
				}
				else
				{
					if (camDist>30)
						camDist -= 1;
					pos.setCoords(XV + camDist*cos(fi2)*cos(fi1),
						YV + camDist*cos(fi2)*sin(fi1),
						(10.5*razmer) + camDist*sin(fi2));
				}

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		if (pos.Z() < 0.1)
			pos.setCoords(pos.X(), pos.Y(), 0.2);
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


			 //Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(0, 0, 400);
	}


	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		//glColor3d(0.9, 0.8, 0);
		//Sphere s;
		//s.pos = pos;
		//s.scale = s.scale*0.08;
		//s.Show();

		//if (OpenGL::isKeyPressed('G'))
		//{
		//	glColor3d(0, 0, 0);
		//	//линия от источника света до окружности
		//	glBegin(GL_LINES);
		//	glVertex3d(pos.X(), pos.Y(), pos.Z());
		//	glVertex3d(pos.X(), pos.Y(), 0);
		//	glEnd();

		//	//рисуем окруность
		//	Circle c;
		//	c.pos.setCoords(pos.X(), pos.Y(), 0);
		//	c.scale = c.scale*1.5;
		//	c.Show();
		//}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 1, 1, 1, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { 1, 1, 1, 0 };
		if (!est_popodanie)
			pos = Vector3(0, 0, 400);
		else
			pos = Vector3(XV, YV, 10 * Rgrib);
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света

		  //старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}


}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == ' ')
	{
		if (BEZIE >= 4 && BEZIE <= 7)
			flag_Bomb_Down = true;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{

	}

	if (key == 'W' && !est_popodanie)//переход вперед
	{
		YV += 4 * razmer*sin(ugolP*M_PI / 180);
		XV += 4 * razmer*cos(ugolP*M_PI / 180);
		if (XV > 100)
			XV = 99;
		if (XV < -100)
			XV = -99;
		if (YV > 100)
			YV = 99;
		if (YV < -100)
			YV = -99;
	}

	if (key == 'S' && !est_popodanie)//сдвиг назад
	{
		YV -= 4 * razmer*sin(ugolP*M_PI / 180);
		XV -= 4 * razmer*cos(ugolP*M_PI / 180);
		if (XV > 100)
			XV = 99;
		if (XV < -100)
			XV = -99;
		if (YV > 100)
			YV = 99;
		if (YV < -100)
			YV = -99;
	}

	if (key == 'A' && !est_popodanie)//сдвижение влево
	{
		YV += 4 * razmer*cos(ugolP*M_PI / 180);
		XV -= 4 * razmer*sin(ugolP*M_PI / 180);
		if (XV > 100)
			XV = 99;
		if (XV < -100)
			XV = -99;
		if (YV > 100)
			YV = 99;
		if (YV < -100)
			YV = -99;
	}

	if (key == 'D' && !est_popodanie)// сдвижение вправо
	{
		YV -= 4 * razmer*cos(ugolP*M_PI / 180);
		XV += 4 * razmer*sin(ugolP*M_PI / 180);
		if (XV > 100)
			XV = 99;
		if (XV < -100)
			XV = -99;
		if (YV > 100)
			YV = 99;
		if (YV < -100)
			YV = -99;
	}

	if (key == 'E' && !est_popodanie)// поворот вправо
	{
		ugolP -= 4 * razmer;
	}

	if (key == 'Q' && !est_popodanie)// подворот влево
	{
		ugolP += 4 * razmer;
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'V' && !alpha)
	{
		lightMode = !lightMode;
		textureMode = !textureMode;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		alpha = true;
	}

	else if (key == 'V' && alpha)
	{
		lightMode = !lightMode;
		textureMode = !textureMode;

		glDisable(GL_BLEND);
		alpha = false;
	}
	if (key == 'C')
	{
		if (flag_alpha)
			flag_alpha = false;
		else
			flag_alpha = true;
	}

}

void keyUpEvent(OpenGL *ogl, int key)
{
	if (key == 'R')
	{
		BEZIE = 1;
		flag_Bomb_Down = false;
		est_popodanie = false;
		razmer = 0.1;
		t = 0;
		t1 = 0;
		Rgrib = 0.1;
		boommax = false;
		XV = 0;
		YV = 0;
		ugolP = 0;
		popolnenie = true;
	}

}




//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);


	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;
	RGBTRIPLE *texarray1;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("bomb.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	/*
	GLuint texId;*/
	//генерируем ИД для текстуры
	glGenTextures(6, texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId[0]);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//---------------------------------------------------------------
	
	char *texCharArray1; //я

	OpenGL::LoadBMP("boom.bmp", &texW, &texH, &texarray1); //я
	OpenGL::RGBtoChar(texarray1, texW, texH, &texCharArray1); //я

	glBindTexture(GL_TEXTURE_2D, texId[1]); //я

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray1); //я

	free(texCharArray1); //я
	free(texarray1); //я


					 //наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//----------------------------------------------------------------

	RGBTRIPLE *texarray2; //я
	char *texCharArray2; //я

	OpenGL::LoadBMP("cube.bmp", &texW, &texH, &texarray2); //я
	OpenGL::RGBtoChar(texarray2, texW, texH, &texCharArray2); //я

	glBindTexture(GL_TEXTURE_2D, texId[2]); //я

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2); //я

	free(texCharArray2); //я
	free(texarray2); //я
					 //наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//------------------------------------------------------------------

	RGBTRIPLE *texarray3; //я
	char *texCharArray3; //я

	OpenGL::LoadBMP("Nebo.bmp", &texW, &texH, &texarray3); //я
	OpenGL::RGBtoChar(texarray3, texW, texH, &texCharArray3); //я

	glBindTexture(GL_TEXTURE_2D, texId[3]); //я

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray3); //я

	free(texCharArray3); //я
	free(texarray3); //я
					 //наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//---------------------------------------------------------------------

	RGBTRIPLE *texarray4; //я
	char *texCharArray4; //я

	OpenGL::LoadBMP("Nos.bmp", &texW, &texH, &texarray4); //я
	OpenGL::RGBtoChar(texarray4, texW, texH, &texCharArray4); //я

	glBindTexture(GL_TEXTURE_2D, texId[4]); //я

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray4); //я

	free(texCharArray4); //я
	free(texarray4); //я
					 //наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//---------------------------------------------------------------------

	RGBTRIPLE *texarray5; //я
	char *texCharArray5; //я

	OpenGL::LoadBMP("Zemlia.bmp", &texW, &texH, &texarray5); //я
	OpenGL::RGBtoChar(texarray5, texW, texH, &texCharArray5); //я

	glBindTexture(GL_TEXTURE_2D, texId[5]); //я

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray5); //я

	free(texCharArray5); //я
	free(texarray5); //я
					 //наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);




	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH);


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}


void normal(double a1[], double b1[], double c1[], double vn[])
{
	double a[] = { a1[0] - b1[0], a1[1] - b1[1], a1[2] - b1[2] };
	double b[] = { c1[0] - b1[0], c1[1] - b1[1], c1[2] - b1[2] };

	vn[0] = a[1] * b[2] - b[1] * a[2];
	vn[1] = -a[0] * b[2] + b[0] * a[2];
	vn[2] = a[0] * b[1] - b[0] * a[1];

	double length = sqrt(pow(vn[0], 2) + pow(vn[1], 2) + pow(vn[2], 2));

	vn[0] /= length;
	vn[1] /= length;
	vn[2] /= length;
}

void tor(GLfloat r, GLfloat R, GLint nsides, GLint rings)
{
	int i, j;
	GLfloat theta, phi, theta1, phi1;
	GLfloat p0[03], p1[3], p2[3], p3[3];
	GLfloat n0[3], n1[3], n2[3], n3[3];

	for (i = 0; i < rings; i++) {
		theta = (GLfloat)i *2.0 * M_PI / rings;
		theta1 = (GLfloat)(i + 1) * 2.0 * M_PI / rings;
		for (j = 0; j < nsides; j++) {
			phi = (GLfloat)j *2.0 * M_PI / nsides;
			phi1 = (GLfloat)(j + 1) * 2.0 * M_PI / nsides;

			p0[0] = cos(theta) * (R + r * cos(phi));
			p0[1] = -sin(theta) * (R + r * cos(phi));
			p0[2] = r * sin(phi);

			p1[0] = cos(theta1) * (R + r * cos(phi));
			p1[1] = -sin(theta1) * (R + r * cos(phi));
			p1[2] = r * sin(phi);

			p2[0] = cos(theta1) * (R + r * cos(phi1));
			p2[1] = -sin(theta1) * (R + r * cos(phi1));
			p2[2] = r * sin(phi1);

			p3[0] = cos(theta) * (R + r * cos(phi1));
			p3[1] = -sin(theta) * (R + r * cos(phi1));
			p3[2] = r * sin(phi1);

			n0[0] = cos(theta) * (cos(phi));
			n0[1] = -sin(theta) * (cos(phi));
			n0[2] = sin(phi);

			n1[0] = cos(theta1) * (cos(phi));
			n1[1] = -sin(theta1) * (cos(phi));
			n1[2] = sin(phi);

			n2[0] = cos(theta1) * (cos(phi1));
			n2[1] = -sin(theta1) * (cos(phi1));
			n2[2] = sin(phi1);

			n3[0] = cos(theta) * (cos(phi1));
			n3[1] = -sin(theta) * (cos(phi1));
			n3[2] = sin(phi1);

			if (GORY==0) 
			{
				glBindTexture(GL_TEXTURE_2D, texId[1]);
				glBegin(GL_QUADS);
				glColor3d(1, 0, 0);
				glNormal3fv(n3);
				glTexCoord2d((24.0 + i*1544.0 / rings) / 2048.0, 1 - (1163.0 + (j + 1)*718.0 / nsides) / 2048.0);
				glVertex3fv(p3);
				glNormal3fv(n2);
				glTexCoord2d((24.0 + (i + 1)*1544.0 / rings) / 2048.0, 1 - (1163.0 + (j + 1)*718.0 / nsides) / 2048.0);
				glVertex3fv(p2);
				glNormal3fv(n1);
				glTexCoord2d((24.0 + (i + 1)*1544.0 / rings) / 2048.0, 1 - (1163.0 + j*718.0 / nsides) / 2048.0);
				glVertex3fv(p1);
				glNormal3fv(n0);
				glTexCoord2d((24.0 + i*1544.0 / rings) / 2048.0, 1 - (1163.0 + j*718.0 / nsides) / 2048.0);
				glVertex3fv(p0);
				glEnd();
			}
			else if (GORY == 1)
			{
				glBindTexture(GL_TEXTURE_2D, texId[1]);
				glBegin(GL_QUADS);
				glColor3d(1, 0, 0);
				glNormal3fv(n3);
				glTexCoord2d((1023.0 + i*995.0 / rings) / 2048.0, 1 - (463.0 + (j + 1)*284.0 / nsides) / 2048.0);
				glVertex3fv(p3);
				glNormal3fv(n2);
				glTexCoord2d((1023.0 + (i+1)*995.0 / rings) / 2048.0, 1 - (463.0 + (j + 1)*284.0 / nsides) / 2048.0);
				glVertex3fv(p2);
				glNormal3fv(n1);
				glTexCoord2d((1023.0 + (i+1)*995.0 / rings) / 2048.0, 1 - (463.0 + j*284.0 / nsides) / 2048.0);
				glVertex3fv(p1);
				glNormal3fv(n0);
				glTexCoord2d((1023.0 + i*995.0 / rings) / 2048.0, 1 - (463.0 + j*284.0 / nsides) / 2048.0);
				glVertex3fv(p0);
				glEnd();
			}
			else if (GORY == 2)
			{
				glBindTexture(GL_TEXTURE_2D, texId[1]);
				glBegin(GL_QUADS);
				glColor3d(1, 0, 0);
				glNormal3fv(n3);
				glVertex3fv(p3);
				glNormal3fv(n2);
				glVertex3fv(p2);
				glNormal3fv(n1);
				glVertex3fv(p1);
				glNormal3fv(n0);
				glVertex3fv(p0);
				glEnd();
			}
		}
	}
}
//-----------------------------------------------------------------
//перерасчитать верхнее основание цилиндра у взрыва
void cilindrBOOM(double X, double Y, double Z, bool osnovaC, double R)
{
	double r = R;
	double x = X, y = Y, z = Z;
	double x1, y1;
	if (osnovaC)
	{
		glBegin(GL_TRIANGLE_FAN);//столб
		glColor3d(1, 0, 1);
		double xxx[] = { x , y ,0 };
		double yyy[] = { r*cos(M_PI / 180), r*sin(M_PI / 180),0 };
		double zzz[] = { r*cos(2 * M_PI / 180), r*sin(2 * M_PI / 180),0 };
		normal(xxx, yyy, zzz, Vector);
		glNormal3dv(Vector);
		glVertex3d(x, y, 0);
		for (int i = 0; i <= 360; i++) {
			x1 = r*cos(i*M_PI / 180);
			y1 = r*sin(i*M_PI / 180);
			glVertex3d(x1, y1, 0);
		}
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 1, 1);
		for (int i = 0; i <= 360; i++) {
			double Point1[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),0 };
			double Point2[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),18 * Rgrib };
			double Point3[] = { r*cos((i + 1)* M_PI / 180), r*sin((i + 1)* M_PI / 180),18 * Rgrib };
			x1 = r*cos(i*M_PI / 180);
			y1 = r*sin(i*M_PI / 180);
			normal(Point3, Point2, Point1, Vector);
			glNormal3dv(Vector);
			glTexCoord2d((619.0 + 301.0*i / 360.0) / 2048.0, 1 - 866.0 / 2048.0);
			glVertex3d(x1, y1, 0);
			glTexCoord2d((619.0 + 301.0*i / 360.0) / 2048.0, 1 - 359.0 / 2048.0);
			glVertex3d(x1, y1, 18 * Rgrib);
		}
		glEnd();

		xxx[2] = 18 * Rgrib;
		yyy[2] = 18 * Rgrib;
		zzz[2] = 18 * Rgrib;
		glBegin(GL_TRIANGLE_FAN);
		normal(xxx, yyy, zzz, Vector);
		glNormal3dv(Vector);
		glColor3d(1, 0, 1);
		glTexCoord2d(588.0 / 2048.0, 1 - 1303.0 / 2048.0);
		glVertex3d(x, y, 18 * Rgrib);
		for (int i = 0; i <= 360; i++) 
		{
			x = r*cos(i*M_PI / 180);
			y = r*sin(i*M_PI / 180);
			glTexCoord2d((588.0 - 101.0*sin(i*M_PI / 180)) / 2048.0, 1 - (1303.0 - 101.0 * cos(i*M_PI / 180)) / 2048.0);
			glVertex3d(x, y, 18 * Rgrib);
		}
		glEnd();
		osnova = false;
	}
	else //----------------------------изменить на предложение игоря
	{
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(1, 0, 1);
		glVertex3d(x, y, 0);
		for (int i = 0; i <= 360; i++) {
			x1 = r*cos(i*M_PI / 180);
			y1 = r*sin(i*M_PI / 180);
			glVertex3d(x1, y1, 0);
		}
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 1, 1);
		for (int i = 0; i <= 360; i++) {
			x1 = r*cos(i*M_PI / 180);
			y1 = r*sin(i*M_PI / 180);
			glVertex3d(x1, y1, 0);
			glVertex3d(x1, y1, 1 * Rgrib);
		}
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glColor3d(1, 0, 1);

		glVertex3d(x, y, 1 * Rgrib);
		for (int i = 0; i <= 360; i++) {
			x = r*cos(i*M_PI / 180);
			y = r*sin(i*M_PI / 180);
			glVertex3d(x, y, 1 * Rgrib);
		}
		glEnd();
		osnova = true;
	}
}

void cilindrSomolet(double X, double Y, double Z, bool osnovaC, double R)
{
	double r = R;
	double x = X, y = Y, z = Z;
	double x1, y1;

	if (osnovaC)
	{
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(1, 0, 1);
		double xxx[] = { x , y ,1.25 };
		double yyy[] = { r*cos(M_PI / 180), r*sin(M_PI / 180),1.25 };
		double zzz[] = { r*cos(2 * M_PI / 180), r*sin(2 * M_PI / 180),1.25 };
		normal(xxx, yyy, zzz, Vector);
		glNormal3dv(Vector);
		glVertex3d(x, y, 1.25);
		for (int i = 0; i <= 360; i++) {
			x1 = r*cos(i*M_PI / 180);
			y1 = r*sin(i*M_PI / 180);
			glVertex3d(x1, y1, 1.25);
		}
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 1, 1);
		for (int i = 0; i <= 360; i++) {
			double Point1[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),1.25 };
			double Point2[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),2.75 };
			double Point3[] = { r*cos((i + 1)* M_PI / 180), r*sin((i + 1)* M_PI / 180),2.75 };
			x1 = r*cos(i*M_PI / 180);
			y1 = r*sin(i*M_PI / 180);
			normal(Point1, Point2, Point3, Vector);
			glNormal3dv(Vector);
			glVertex3d(x1, y1, 1.25);
			glVertex3d(x1, y1, 2.75);
		}
		glEnd();

		xxx[2] = 2.75;
		yyy[2] = 2.75;
		zzz[2] = 2.75;
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(1, 0, 1);
		normal(zzz, yyy, xxx, Vector);
		glNormal3dv(Vector);
		glVertex3d(x, y, 2.75);
		for (int i = 0; i <= 360; i++) {
			x = r*cos(i*M_PI / 180);
			y = r*sin(i*M_PI / 180);
			glVertex3d(x, y, 2.75);
		}
		glEnd();
		osnovaSomolet = false;
	}
	else
	{
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(1, 0, 1);
		double xxx[] = { x , y ,1.5 };
		double yyy[] = { r*cos(M_PI / 180), r*sin(M_PI / 180),1.5 };
		double zzz[] = { r*cos(2 * M_PI / 180), r*sin(2 * M_PI / 180),1.5 };
		normal(xxx, yyy, zzz, Vector);
		glNormal3dv(Vector);
		glVertex3d(x, y, 1.5);
		for (int i = 0; i <= 360; i++) {
			x1 = r*cos(i*M_PI / 180);
			y1 = r*sin(i*M_PI / 180);
			glVertex3d(x1, y1, 1.5);
		}
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 1, 1);
		for (int i = 0; i <= 360; i++) {
			double Point1[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),1.5 };
			double Point2[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),2.5 };
			double Point3[] = { r*cos((i + 1)* M_PI / 180), r*sin((i + 1)* M_PI / 180),2.5 };
			x1 = r*cos(i*M_PI / 180);
			y1 = r*sin(i*M_PI / 180);
			normal(Point1, Point2, Point3, Vector);
			glNormal3dv(Vector);
			glVertex3d(x1, y1, 1.5);
			glVertex3d(x1, y1, 2.5);
		}
		glEnd();

		xxx[2] = 2.5;
		yyy[2] = 2.5;
		zzz[2] = 2.5;
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(1, 0, 1);
		normal(zzz, yyy, xxx, Vector);
		glNormal3dv(Vector);
		glVertex3d(x, y, 2.5);
		for (int i = 0; i <= 360; i++) {
			x = r*cos(i*M_PI / 180);
			y = r*sin(i*M_PI / 180);
			glVertex3d(x, y, 2.5);
		}
		glEnd();
		osnovaSomolet = true;
	}
}

void cilindrSneg(double X, double Y, double Z, double R)
{
	double r = R;
	double x = X, y = Y, z = Z;
	double x1, y1;

	glBegin(GL_TRIANGLE_FAN);
	glColor3d(1, 0, 1);
	double xxx[] = { x , y ,0 };
	double yyy[] = { r*cos(M_PI / 180), r*sin(M_PI / 180),0 };
	double zzz[] = { r*cos(2 * M_PI / 180), r*sin(2 * M_PI / 180),0 };
	normal(zzz, yyy, xxx, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(125.0/256.0, 1-36.0/256.0);
	glVertex3d(x, y, 0);
	for (int i = 0; i <= 360; i++) {
		x1 = r*1.5*cos(i*M_PI / 180);
		y1 = r*1.5*sin(i*M_PI / 180);
		glTexCoord2d((125.0-15.0*sin(i*M_PI / 180))/256, 1-(36.0 - 15.0 * cos(i*M_PI / 180))/256);
		glVertex3d(x1, y1, 0);
	}
	glEnd();

	xxx[2] = -0.01*razmer;
	yyy[2] = -0.01*razmer;
	zzz[2] = -0.01*razmer;
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(1, 0, 1);
	normal(xxx, yyy, zzz, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(125.0 / 256.0, 1 - 36.0 / 256.0);
	glVertex3d(x, y, -0.01*razmer);
	for (int i = 0; i <= 360; i++) {
		x1 = r*1.5*cos(i*M_PI / 180);
		y1 = r*1.5*sin(i*M_PI / 180);
		glTexCoord2d((125.0 - 15.0*sin(i*M_PI / 180)) / 256, 1 - (36.0 - 15.0 * cos(i*M_PI / 180)) / 256);
		glVertex3d(x1, y1, -0.01*razmer);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 1);
	for (int i = 0; i <= 360; i++) {
		double Point1[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),0 };
		double Point2[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),1.25*razmer };
		double Point3[] = { r*cos((i + 1)* M_PI / 180), r*sin((i + 1)* M_PI / 180),1.25*razmer };
		x1 = r*cos(i*M_PI / 180);
		y1 = r*sin(i*M_PI / 180);
		normal(Point1, Point2, Point3, Vector);
		glNormal3dv(Vector);
		glTexCoord2d((21.0 + 140.0*i / 360.0) / 256.0, 1 - 137.0 / 256.0);
		glVertex3d(x1, y1, 0);
		glTexCoord2d((21.0 + 140.0*i / 360.0) / 256.0, 1 - 101.0 / 256.0);
		glVertex3d(x1, y1, 1.25*razmer);
	}
	glEnd();

	xxx[2] = 1.25*razmer;
	yyy[2] = 1.25*razmer;
	zzz[2] = 1.25*razmer;
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(1, 0, 1);
	normal(zzz, yyy, xxx, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(91.0 / 256.0, 1 - 155.0 / 256.0);
	glVertex3d(x, y, 1.25*razmer);
	for (int i = 0; i <= 360; i++) {
		x = r*cos(i*M_PI / 180);
		y = r*sin(i*M_PI / 180);
		glTexCoord2d((91.0 - 11.0*sin(i*M_PI / 180)) / 256, 1 - (155.0 - 11.0 * cos(i*M_PI / 180)) / 256);
		glVertex3d(x, y, 1.25*razmer);
	}
	glEnd();
}

void cilindrNos(double X, double Y, double Z, double R)
{
	double r = R;
	double x = X, y = Y, z = Z;
	double x1, y1;

	glBegin(GL_TRIANGLE_FAN);
	glColor3d(1, 0, 1);
	double xxx[] = { x , y ,0 };
	double yyy[] = { r*cos(M_PI / 180), r*sin(M_PI / 180),0 };
	double zzz[] = { r*cos(2 * M_PI / 180), r*sin(2 * M_PI / 180),0 };
	normal(zzz, yyy, xxx, Vector);
	glNormal3dv(Vector);
	glVertex3d(x, y, 0);
	for (int i = 0; i <= 360; i++) {
		x1 = r*1.5*cos(i*M_PI / 180);
		y1 = r*1.5*sin(i*M_PI / 180);
		glVertex3d(x1, y1, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 1);
	for (int i = 0; i <= 360; i++) {
		double Point1[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),0 };
		double Point2[] = { r*cos(i*M_PI / 180), r*sin(i*M_PI / 180),1.5*razmer };
		double Point3[] = { r*cos((i + 1)* M_PI / 180), r*sin((i + 1)* M_PI / 180),1.5 *razmer };
		x1 = r*cos(i*M_PI / 180);
		y1 = r*sin(i*M_PI / 180);
		normal(Point1, Point2, Point3, Vector);
		glNormal3dv(Vector);
		glTexCoord2d((20.0+38.0*i/360.0)/256.0 , 1-219.0/256.0);
		glVertex3d(x1, y1, 0);
		glTexCoord2d((20.0 + 38.0 * i / 360.0) / 256.0, 1 - 173.0 / 256.0);
		glVertex3d(x, y, 1.5*razmer);
	}
	glEnd();
}


void bomb()
{
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	double S1[] = { 0,0,0 };
	double S2[] = { 0,0, 0.6 };
	double S3[] = { 0,0,0.8 };
	double A[] = { 0.2,-0.2,0.2 };
	double A1[] = { 0.2,-0.2,0.6 };
	double B[] = { 0.2,0.2,0.2 };
	double B1[] = { 0.2,0.2,0.6 };
	double C[] = { -0.2,0.2,0.2 };
	double C1[] = { -0.2,0.2,0.6 };
	double D[] = { -0.2,-0.2,0.2 };
	double D1[] = { -0.2,-0.2,0.6 };
	double E[] = { 0.1,0.0,0.8 };
	double F[] = { 0,0.1,0.8 };
	double G[] = { -0.1,0.0,0.8 };
	double H[] = { 0,-0.1,0.8 };

	glRotated(180, 0, 1, 0);

	glBegin(GL_TRIANGLES);
	glColor3d(1, 1, 1);
	normal(B, S1, A, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(40.0/256.0, 1-20.0/256.0);
	glVertex3dv(S1);
	glTexCoord2d(20.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(A);
	glTexCoord2d(60.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(B);

	normal(C, S1, B, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(105.0 / 256.0, 1 - 20.0 / 256.0);
	glVertex3dv(S1);
	glTexCoord2d(85.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(B);
	glTexCoord2d(125.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(C);

	normal(D, S1, C, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(164.0 / 256.0, 1 - 20.0 / 256.0);
	glVertex3dv(S1);
	glTexCoord2d(144.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(C);
	glTexCoord2d(184.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(D);

	normal(A, S1, D, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(225.0 / 256.0, 1 - 20.0 / 256.0);
	glVertex3dv(S1);
	glTexCoord2d(205.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(D);
	glTexCoord2d(245.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(A);


	glColor3d(1, 0, 0);
	normal(F, S2, E, Vector);
	glNormal3dv(Vector);
	glTexCoord2d( 105.0/ 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(S2);
	glTexCoord2d(95.0 / 256.0, 1 - 109.0 / 256.0);
	glVertex3dv(E);
	glTexCoord2d(115.0 / 256.0, 1 - 109.0 / 256.0);
	glVertex3dv(F);

	normal(G, S2, F, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(164.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(S2);
	glTexCoord2d(154.0 / 256.0, 1 - 109.0 / 256.0);
	glVertex3dv(F);
	glTexCoord2d(174.0 / 256.0, 1 - 109.0 / 256.0);
	glVertex3dv(G);

	normal(H, S2, G, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(225.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(S2);
	glTexCoord2d(215.0 / 256.0, 1 - 109.0 / 256.0);
	glVertex3dv(G);
	glTexCoord2d(235.0 / 256.0, 1 - 109.0 / 256.0);
	glVertex3dv(H);

	normal(E, S2, H, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(40.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(S2);
	glTexCoord2d(30.0 / 256.0, 1 - 109.0 / 256.0);
	glVertex3dv(H);
	glTexCoord2d(50.0 / 256.0, 1 - 109.0 / 256.0);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
	normal(B1, A, A1, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(20.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(A);
	glTexCoord2d(20.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(A1);
	glTexCoord2d(60.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(B1);
	glTexCoord2d(60.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(B);

	normal(C1, B, B1, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(85.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(B);
	glTexCoord2d(85.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(B1);
	glTexCoord2d(125.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(C1);
	glTexCoord2d(125.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(C);

	normal(D1, C, C1, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(144.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(C);
	glTexCoord2d(144.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(C1);
	glTexCoord2d(184.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(D1);
	glTexCoord2d(184.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(D);

	normal(A1, D, D1, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(205.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(D);
	glTexCoord2d(205.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(D1);
	glTexCoord2d(245.0 / 256.0, 1 - 49.0 / 256.0);
	glVertex3dv(A1);
	glTexCoord2d(245.0 / 256.0, 1 - 89.0 / 256.0);
	glVertex3dv(A);

	normal(C1, B1, A1, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(61.0 / 256.0, 1 - 125.0 / 256.0);
	glVertex3dv(A1);
	glTexCoord2d(100.0 / 256.0, 1 - 125.0 / 256.0);
	glVertex3dv(B1);
	glTexCoord2d(100.0 / 256.0, 1 - 164.0 / 256.0);
	glVertex3dv(C1);
	glTexCoord2d(61.0 / 256.0, 1 - 164.0 / 256.0);
	glVertex3dv(D1);


	normal(G, F, E, Vector);
	glNormal3dv(Vector);
	glTexCoord2d(30.0 / 256.0, 1 - 125.0 / 256.0);
	glVertex3dv(E);
	glTexCoord2d(40.0 / 256.0, 1 - 135.0 / 256.0);
	glVertex3dv(F);
	glTexCoord2d(30.0 / 256.0, 1 - 145.0 / 256.0);
	glVertex3dv(G);
	glTexCoord2d(20.0 / 256.0, 1 - 135.0 / 256.0);
	glVertex3dv(H);

	glEnd();

	glRotated(-180, 0, 1, 0);

}
//пока есть
void somolet()
{
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.54, 0.58, 0.58, 1. };
	GLfloat spec[] = { 0.54, 0.58, 0.58, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);
	
	cilindrSomolet(0, 0, 0, osnovaSomolet, 1);
	cilindrSomolet(0, 0, 0, osnovaSomolet, 2);
	glTranslated(0, 0, 2);
	GORY = 2;
	tor(0.5, 2, 90, 90);
	glTranslated(0, 0, -2);
}
//пока есть
void boom()
{
	glBindTexture(GL_TEXTURE_2D, texId[1]);
	glTranslated(XV, YV, 0);
	cilindrBOOM(0, 0, ZV, osnova, 3 * Rgrib);//столб
	cilindrBOOM(0, 0, ZV, osnova, 7.5* Rgrib);//основа
	glTranslated(0, 0, 18 * Rgrib);
	GORY = 0;
	tor(3 * Rgrib, 6 * Rgrib, 90, 90);
	glTranslated(0, 0, -18 * Rgrib);
	glTranslated(0, 0, 21 * Rgrib);
	GORY = 1;
	tor(1.5* Rgrib, 12 * Rgrib, 90, 90);
	glTranslated(-XV, -YV, -21 * Rgrib);

}
//пока есть
void cube()
{
	double AC[] = { 200,-200,0 };
	double BC[] = { -200,-200,0 };
	double CC[] = { -200,200,0 };
	double DC[] = { 200,200,0 };
	double A1C[] = { 200,-200,50 };
	double B1C[] = { -200,-200,50 };
	double C1C[] = { -200,200,50 };
	double D1C[] = { 200,200,50 };

	glBindTexture(GL_TEXTURE_2D, texId[3]);
	glBegin(GL_POLYGON);//1
	glColor3f(1, 1, 1);
	if (!est_popodanie)
	normal(A1C, B1C, D1C, Vector);
	else
	normal(D1C, B1C, A1C, Vector);
	glNormal3dv(Vector);
	glTexCoord2i(0,0);
	glVertex3dv(A1C);
	glTexCoord2d(0,50);
	glVertex3dv(B1C);
	glTexCoord2d(50,50);
	glVertex3dv(C1C);
	glTexCoord2d(50,0);
	glVertex3dv(D1C);
	glEnd();


	glBegin(GL_POLYGON);//2
	glColor3f(0.5, 1.0, 0.4);
	normal(BC, B1C, CC, Vector);
	glNormal3dv(Vector);
	glVertex3dv(BC);
	glVertex3dv(B1C);
	glVertex3dv(C1C);
	glVertex3dv(CC);
	glEnd();

	glBegin(GL_POLYGON);//3
	glColor3f(0.5, 1.0, 0.4);
	normal(DC, CC, D1C, Vector);
	glNormal3dv(Vector);
	glVertex3dv(DC);
	glVertex3dv(CC);
	glVertex3dv(C1C);
	glVertex3dv(D1C);
	glEnd();

	glBegin(GL_POLYGON);//4
	glColor3f(0.5, 1.0, 0.4);
	normal(DC, A1C, AC, Vector);
	glNormal3dv(Vector);
	glVertex3dv(AC);
	glVertex3dv(A1C);
	glVertex3dv(D1C);
	glVertex3dv(DC);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId[5]);
	glBegin(GL_POLYGON);//5
	glColor3f(1, 1, 1);
	normal(BC, CC, DC, Vector);
	glNormal3dv(Vector);
	glTexCoord2i(0, 0);
	glVertex3dv(AC);
	glTexCoord2i(0, 1000);
	glVertex3dv(BC);
	glTexCoord2i(1000, 1000);
	glVertex3dv(CC);
	glTexCoord2i(1000, 0);
	glVertex3dv(DC);
	glEnd();

	glBegin(GL_POLYGON);//6
	glColor3f(0.5, 1.0, 0.4);
	normal(A1C, BC, AC, Vector);
	glNormal3dv(Vector);
	glVertex3dv(AC);
	glVertex3dv(BC);
	glVertex3dv(B1C);
	glVertex3dv(A1C);
	glEnd();
}

void sphere(double r, int nx, int ny)
{
	int ix, iy;
	double x, y, z, sy, cy, sy1, cy1, sx, cx, piy, pix, ay, ay1, ax, tx, ty, ty1, dnx, dny, diy;
	dnx = 1.0 / (double)nx;
	dny = 1.0 / (double)ny;
	// Рисуем полигональную модель сферы, формируем нормали и задаем коодинаты текстуры
	// Каждый полигон - это трапеция. Трапеции верхнего и нижнего слоев вырождаются в треугольники
	glBegin(GL_QUAD_STRIP);
	piy = PI * dny;
	pix = PI * dnx;
	for (iy = 0; iy < ny; iy++)
	{
		diy = (double)iy;
		ay = diy * piy;
		sy = sin(ay);
		cy = cos(ay);
		ty = diy * dny;
		ay1 = ay + piy;
		sy1 = sin(ay1);
		cy1 = cos(ay1);
		ty1 = ty + dny;
		for (ix = 0; ix <= nx; ix++)
		{
			ax = 2.0 * ix * pix;
			sx = sin(ax);
			cx = cos(ax);
			x = r * sy * cx;
			y = r * sy * sx;
			z = r * cy;
			tx = (double)ix * dnx;
			// Координаты нормали в текущей вершине
			glNormal3d(x, y, z); // Нормаль направлена от центра
								 // Координаты текстуры в текущей вершине
								 //GL.TexCoord2(tx, ty);
			glVertex3d(x, y, z);
			x = r * sy1 * cx;
			y = r * sy1 * sx;
			z = r * cy1;
			glNormal3d(x, y, z);
			//GL.TexCoord2(tx, ty1);
			glVertex3d(x, y, z);
		}
	}
	glEnd();
}

void snegovik()
{

	glTranslated(0, 0, 3 * razmer);
	sphere(3 * razmer, 50, 50);
	glTranslated(0, 0, 0);

	glTranslated(0, 0, 4 * razmer);
	sphere(2 * razmer, 50, 50);
	glTranslated(0, 0, 0);

	glTranslated(0, 0, 2.5*razmer);
	sphere(1 * razmer, 50, 50);
	glTranslated(0, 0, 0);

	glTranslated(0.8*razmer, 0.5*razmer, 0.3*razmer);
	sphere(0.1*razmer, 50, 50);
	glTranslated(-0.8*razmer, -0.5*razmer, 0);

	glTranslated(0.8*razmer, -0.5*razmer, 0);
	sphere(0.1*razmer, 50, 50);
	glTranslated(-0.8*razmer, 0.5*razmer, 0);
	glBindTexture(GL_TEXTURE_2D, texId[4]);
	glTranslated(0.7*razmer, 0, -0.2*razmer);
	glRotated(90, 0, 1, 0);
	cilindrNos(0, 0, 0, 0.2*razmer);
	glRotated(-90, 0, 1, 0);
	glTranslated(-0.7*razmer, 0, 0);

	glTranslated(0, 0, 0.7*razmer);
	cilindrSneg(0, 0, 0, 0.75*razmer);
	glTranslated(0, 0, -10.3*razmer);
}

//------------------------------------начало функций для безье

void bezie_move(double P1[], double P2[], double P3[], double P4[], double Point[], double &t)
{
	Point[0] = pow(1 - t, 3)*P1[0] + 3 * t*pow(1 - t, 2)*P2[0] + 3 * t*t*(1 - t)*P3[0] + t*t*t*P4[0];
	Point[1] = pow(1 - t, 3)*P1[1] + 3 * t*pow(1 - t, 2)*P2[1] + 3 * t*t*(1 - t)*P3[1] + t*t*t*P4[1];
	Point[2] = pow(1 - t, 3)*P1[2] + 3 * t*pow(1 - t, 2)*P2[2] + 3 * t*t*(1 - t)*P3[2] + t*t*t*P4[2];
	if (!est_popodanie)
		t += 0.01;
	else
		t += 0.2;
}

double length(double a[])
{
	return pow(pow(a[0], 2) + pow(a[1], 2) + pow(a[2], 2), 0.5);
}

void line(double A[], double B[], double RGB[])
{
	glColor3dv(RGB);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3dv(A);
	glVertex3dv(B);
	glEnd();
	glLineWidth(1);
}

void bezie(double P1[], double P2[], double P3[], double P4[], double RGB1[], double RGB2[])
{
	double PointPrev[] = { P1[0],P1[1],P1[2] };

	//line(P1, P2, RGB1);
	//line(P2, P3, RGB1);
	//line(P3, P4, RGB1);

	for (double t = 0; t <= 1; t += 0.01)
	{
		double PointCur[3] = { 0 };
		PointCur[0] = pow(1 - t, 3)*P1[0] + 3 * t*pow(1 - t, 2)*P2[0] + 3 * t*t*(1 - t)*P3[0] + t*t*t*P4[0];
		PointCur[1] = pow(1 - t, 3)*P1[1] + 3 * t*pow(1 - t, 2)*P2[1] + 3 * t*t*(1 - t)*P3[1] + t*t*t*P4[1];
		PointCur[2] = pow(1 - t, 3)*P1[2] + 3 * t*pow(1 - t, 2)*P2[2] + 3 * t*t*(1 - t)*P3[2] + t*t*t*P4[2];
		//line(PointPrev, PointCur, RGB2);
		PointPrev[0] = PointCur[0]; PointPrev[1] = PointCur[1]; PointPrev[2] = PointCur[2];
	}
}
//---------------------конец функций безье

void bezpostr(double P0[], double P1[], double P2[], double P3[], double &t)
{
	double skor = 0.01;
	double C1[] = { 1, 1, 0 };
	double C2[] = { 0, 1, 1 };
	double C3[] = { 1, 0, 1 };
	double C4[] = { 1, 0, 0 };
	double P01[] = { P0[0] + (P1[0] - P0[0])*t, P0[1] + (P1[1] - P0[1])*t, P0[2] + (P1[2] - P0[2])*t };
	double P12[] = { P1[0] + (P2[0] - P1[0])*t, P1[1] + (P2[1] - P1[1])*t, P1[2] + (P2[2] - P1[2])*t };
	double P23[] = { P2[0] + (P3[0] - P2[0])*t, P2[1] + (P3[1] - P2[1])*t, P2[2] + (P3[2] - P2[2])*t };
	double P012[] = { P01[0] + (P12[0] - P01[0])*t, P01[1] + (P12[1] - P01[1])*t, P01[2] + (P12[2] - P01[2])*t };
	double P123[] = { P12[0] + (P23[0] - P12[0])*t, P12[1] + (P23[1] - P12[1])*t, P12[2] + (P23[2] - P12[2])*t };
	double P0123[] = { P012[0] + (P123[0] - P012[0])*t, P012[1] + (P123[1] - P012[1])*t, P012[2] + (P123[2] - P012[2])*t };
	double V0123[] = { P012[0] + (P123[0] - P012[0])*(t - skor), P012[1] + (P123[1] - P012[1])*(t - skor), P012[2] + (P123[2] - P012[2])*(t - skor) };
	double R1 = 1;
	double A[] = { 1, 1, 0 };
	double B[] = { 1, -1, 0 };
	double C[] = { -1, -1, 0 };
	double D[] = { -1, 1, 0 };

	//bezie(P0, P1, P2, P3,C1,C2);

	glPushMatrix();
	double Vec[] = { P123[0] - P012[0], P123[1] - P012[1], P123[2] - P012[2] };
	double V012[] = { P012[0] + Vec[0] * t, P012[1] + Vec[1] * t, P012[2] + Vec[2] * t };
	double V123[] = { P012[0] + Vec[0] * (t + 0.5), P012[1] + Vec[1] * (t + 0.5), P012[2] + Vec[2] * (t + 0.5) };

	//line(V012, V123, C4);

	glTranslated(V012[0], V012[1], V012[2]);
	double P[] = { Vec[0] * 3, Vec[1] * 3, Vec[2] * 3 };
	double angle = acos(Vec[2] / (pow((pow(Vec[0], 2) + pow(Vec[1], 2) + pow(Vec[2], 2)), 0.5)));
	glPushMatrix();
	glRotated(angle * 180 / M_PI, -Vec[1], Vec[0], 0);
	glColor3d(0.8, 0.8, 0.8);
	bomb();
	glRotated(-angle * 180 / M_PI, -Vec[1], Vec[0], 0);
	glPopMatrix();
	glTranslated(-V012[0], -V012[1], -V012[2]);
	glPopMatrix();

	PointEnd[0] = V012[0];
	PointEnd[1] = V012[1];
	PointEnd[2] = V012[2];

	if (t <= 1)
		t = t + skor;

}

void Render(OpenGL *ogl)
{
	glBindTexture(GL_TEXTURE_2D, texId[0]);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//альфаналожение
	if (flag_alpha)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 1, 1, 1, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Координаты текстур

	//-------------------------
	//моя прога
	if (popolnenie)
	{
		for (int i = 0; i < 16; i++)
		{
			masbezieX[i] = -150 + rand() % 300;
			masbezieY[i] = -150 + rand() % 300;
		}
		popolnenie = false;
	}

	double P11[] = { -5, 25, 200 };//спуск/подъем
	double P12[] = { -10,20,20 };
	double P13[] = { 10,22,20 };
	double P14[] = { 10,15,20 };

	double RGB4[] = { 1,0.6,0 };

	double P21[] = { 10,15,20 };//начало петли
	double P22[] = { 10,10,20 };
	double P23[] = { 10,10,25 };
	double P24[] = { 10,15,25 };

	double P31[] = { 10,15,25 };
	double P32[] = { 10,20,25 };
	double P33[] = { 10,20,20 };
	double P34[] = { 10,5,20 };//конец петли

	double P41[] = { 10,5,20 };
	double P42[] = { masbezieX[1],masbezieY[1],20 };
	double P43[] = { masbezieX[2],masbezieY[2],20 };
	double P44[] = { masbezieX[3],masbezieY[3],20 };

	double P51[] = { masbezieX[3],masbezieY[3],20 };
	double P52[] = { masbezieX[5],masbezieY[5],20 };
	double P53[] = { masbezieX[6],masbezieY[6],20 };
	double P54[] = { masbezieX[7],masbezieY[7],200 };

	double P61[] = { masbezieX[7],masbezieY[7],20 };
	double P62[] = { masbezieX[9],masbezieY[9],20 };
	double P63[] = { masbezieX[10],masbezieY[10],20 };
	double P64[] = { masbezieX[11],masbezieY[11],20 };

	double P71[] = { masbezieX[11],masbezieY[11],20 };
	double P72[] = { masbezieX[13],masbezieY[13],20 };
	double P73[] = { masbezieX[14],masbezieY[14],20 };
	double P74[] = { 10,15,20 };


	double P0[3] = { 0 };

	double PSneg[] = { XV ,YV, 0 };

	double RGB1[] = { 1,0,0 };
	double RGB5[] = { 0,1,0.3 };
	bezie(P11, P12, P13, P14, RGB1, RGB5);
	bezie(P21, P22, P23, P24, RGB1, RGB5);
	bezie(P31, P32, P33, P34, RGB1, RGB5);
	bezie(P41, P42, P43, P44, RGB1, RGB5);
	bezie(P51, P52, P53, P54, RGB1, RGB5);

	double angleXY;
	double angleXZ;
	double angleYZ;

	glPushMatrix();
	if (BEZIE == 1)
	{
		if (t <= 1)
		{
			bezie_move(P11, P12, P13, P14, PointCur, t);
			if (t >= 1)
			{
				t = 0;
				BEZIE = 2;
			}
		}
	}
	else
		if (BEZIE == 2)
		{
			if (t <= 1)
			{
				bezie_move(P21, P22, P23, P24, PointCur, t);
				if (t >= 1)
				{
					t = 0;
					BEZIE = 3;
				}
			}
		}
		else
			if (BEZIE == 3)
			{
				if (t <= 1)
				{
					bezie_move(P31, P32, P33, P34, PointCur, t);
					if (t >= 1)
					{
						t = 0;
						BEZIE = 4;
					}
				}
			}
			else
				if (BEZIE == 4)
				{
					if (t <= 1)
					{
						bezie_move(P41, P42, P43, P44, PointCur, t);
						if (t >= 1)
						{
							t = 0;
							BEZIE = 5;
						}
					}
				}
				else
					if (BEZIE == 5)
					{
						if (t <= 1)
						{
							bezie_move(P51, P52, P53, P54, PointCur, t);
							if (t >= 1)
							{
								t = 0;
								BEZIE = 6;
							}
						}
					}
					else
						if (BEZIE == 6)
						{
							if (t <= 1)
							{
								bezie_move(P61, P62, P63, P64, PointCur, t);
								if (t >= 1)
								{
									t = 0;
									BEZIE = 7;
								}
							}
						}
						else
							if (BEZIE == 7)
							{
								if (t <= 1)
								{
									bezie_move(P71, P72, P73, P74, PointCur, t);
									if (t >= 1)
									{
										t = 0;
										BEZIE = 8;
									}
								}
							}
							else
								if (BEZIE == 8)
								{
									if (t <= 1)
									{
										bezie_move(P14, P13, P12, P11, PointCur, t);
										if (t >= 1)
										{
											if (!flag_Bomb_Down)
											{
												t = 0;
												BEZIE = 1;
											}
										}
									}
								}

	glPushMatrix();
	double vec[] = { PointCur[0] - PointPrev[0],PointCur[1] - PointPrev[1],PointCur[2] - PointPrev[2] };
	vec[0] /= length(vec);
	vec[1] /= length(vec);
	vec[2] /= length(vec);
	glTranslated(PointCur[0], PointCur[1], PointCur[2]);
	double P[] = { vec[0] * 3,vec[1] * 3,vec[2] * 3 };
	double angle = acos(vec[2] / (pow((pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2)), 0.5)));
	glPushMatrix();
	glRotated(-90, -vec[1], vec[0], 0);
	glRotated(angle * 180 / 3.14, -vec[1], vec[0], 0);
	glTranslated(0, 0, -1.5);
	somolet();
	glPopMatrix();
	//line(P0, P, RGB4);
	glPopMatrix();
	glPopMatrix();
	PointPrev[0] = PointCur[0];
	PointPrev[1] = PointCur[1];
	PointPrev[2] = PointCur[2];
	rendCount++;

	if (!flag_Bomb_Down)
	{
		xb = PointCur[0];
		yb = PointCur[1];
		zb = PointCur[2];
	}
	double P00[] = { xb,yb,zb };


	if (flag_Bomb_Down)
	{
		if (t1 <= 1)
		{
			bezpostr(P00, P00, P, PSneg, t1);
			//bomb();
		}
		else
		{
			if (razmer<1)
				est_popodanie = true;
		}
	}



	cube();
	double color1[] = { 1,0,0 };
	glColor3dv(color1);
	if (est_popodanie)
	{
		boom();
		if (Rgrib < 1 && !boommax)
		{
			Rgrib += 0.01;
		}
		else
		{
			boommax = true;
			if (Rgrib > 0.1)
			{
				Rgrib -= 0.02;
			}
			if (razmer < 1)
				razmer += 0.02;
			else
				est_popodanie = false;
		}
	}
	if (!est_popodanie || boommax)
	{
		glTranslated(XV, YV, 0);
		glRotated(ugolP, 0, 0, 1);
		snegovik();
		glRotated(-ugolP, 0, 0, 1);
		glTranslated(-XV, -YV, 0);
	}

	//конец моей проги


	////текст сообщения вверху слева, если надоест - закоментировать, или заменить =)
	//char c[250];  //максимальная длина сообщения
	//sprintf_s(c, "(T)Текстуры - %d\n(L)Свет - %d\n\nУправление светом:\n"
	//	"G - перемещение в горизонтальной плоскости,\nG+ЛКМ+перемещение по вертикальной линии\n"
	//	"R - установить камеру и свет в начальное положение\n"
	//	"F - переместить свет в точку камеры", textureMode, lightMode);
	//ogl->message = std::string(c);

}   //конец тела функции
