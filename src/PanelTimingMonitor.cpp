#include "PanelTimingMonitor.h"
#include "Configuration.h"
#include "Logger.h"



PanelTimingMonitor::PanelTimingMonitor(std::string a_sPanelName):
m_sPanelName(a_sPanelName),
m_pCrystalBallFunction(new TF1("CrystalBall", this, &PanelTimingMonitor::CrystalBall,-100,100,5)),
m_iFitCounter(0),
m_iMaxFitCounter(Configuration::Instance().GetNumberOfEventsToFitAfter())
{}

void PanelTimingMonitor::GotEvent(int a_iChannelNum, float a_fTimeOfPulseEdge, float a_fPulseAmplitude, float a_fTriggerEdge)
{
	//printf("Got event");
	if(m_pCanvas == NULL)
	{
		InitGraphics();
	}
	
	//printf("Panel %s, time of pulse: %f, time of trigger: %f\n", m_sPanelName.c_str(), a_fTimeOfPulseEdge, a_fTriggerEdge);
	m_pCanvas->cd();
	m_pTimingHist->Fill(a_fTimeOfPulseEdge - a_fTriggerEdge);
	m_pTimingHist->Draw(); 
	m_pTimingHist->SetCanExtend(TH1::kXaxis);

	m_pCanvas->Update();
	if(m_iMaxFitCounter > 0)
	{
		if (m_iMaxFitCounter <= m_iFitCounter)
		{
			m_iFitCounter = 0;
			m_pTimingHist->Fit("CrystalBall", "LW");
		}
		else
		{
			m_iFitCounter ++;
		}
	}
	
	m_vpLineHistograms[a_iChannelNum]->Fill(a_fTimeOfPulseEdge - a_fTriggerEdge);
	m_vpLineHistograms[a_iChannelNum]->Draw();
	m_vpLineHistograms[a_iChannelNum]->SetCanExtend(TH1::kXaxis);
	m_pLineTimingCanvas->Update();
}

void PanelTimingMonitor::InitGraphics()
{
	printf("INITGRAPHICS\n");
	
	if(Configuration::Instance().ShouldFitTimingHistograms())
	{
	   	m_pCrystalBallFunction->SetParameters(0.46,15.67,22,5.4, 45);
	   	m_pCrystalBallFunction->SetParNames("alpha","n", "mu","sigma","N");
	}
	else
	{
		m_iMaxFitCounter = 0;
	}

	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas((m_sPanelName + "_PanelTimingMonitor").c_str(), (std::string("Panel ") + m_sPanelName  + " Timing Monitor").c_str(), 800, 600));
	m_pTimingHist = new TH1F((m_sPanelName + "TimingHist").c_str() , (std::string("Pan. ") + m_sPanelName + " Timing (Pulse-Trigger)").c_str(), 100, 0, 0);
	m_pTimingHist->SetFillColor(49);
	m_pTimingHist->GetXaxis()->SetTitle("Time (ns)");
	m_pTimingHist->GetXaxis()->CenterTitle();

	m_pLineTimingCanvas = std::unique_ptr<TCanvas>(new TCanvas((m_sPanelName + "_LineTimingMonitor").c_str(), (std::string("Panel ") + m_sPanelName  + " Line Timing Monitor").c_str(), 800, 600));
	std::vector<int> vChannelRange = Configuration::Instance().GetRanges()[m_sPanelName];
	int iNumberOfLines = vChannelRange.size();
	if (iNumberOfLines > 1)
	{
// determine the division of the canvas into pads according to the number of lines
		double fPads1 = ceil(sqrt(iNumberOfLines));
		double fPads2 = ceil(iNumberOfLines/fPads1);

// since the screen is wider usually, we want more pads in the horizontal direction:
		if ( fPads1 > fPads2)
		{
			m_pLineTimingCanvas->Divide((int)fPads1, (int)fPads2);	
		}
		else
		{
			m_pLineTimingCanvas->Divide((int)fPads2, (int)fPads1);
		}
	}

	for (int i = 0; i < iNumberOfLines; i++)
	{
		m_pLineTimingCanvas->cd(i + 1);
		TH1* hist = new TH1F((m_sPanelName + "_" + std::to_string(vChannelRange[i])).c_str(), (std::string("Line ") + std::to_string(vChannelRange[i])).c_str(), 100, 0, 0);
		m_vpLineHistograms[vChannelRange[i]] = hist;
		m_vpLineHistograms[vChannelRange[i]]->SetFillColor(49);
		m_vpLineHistograms[vChannelRange[i]]->GetXaxis()->SetTitle("Time (ns)");
		m_vpLineHistograms[vChannelRange[i]]->GetXaxis()->CenterTitle();
		hist->Draw();
		hist->SetCanExtend(TH1::kXaxis);
	}
//	m_pChannelsHist->SetStats(0);
//	m_pCanvas->Divide(1,2);

}

double PanelTimingMonitor::CrystalBall(double* x, double* par){ 
//http://en.wikipedia.org/wiki/Crystal_Ball_function 
	double xcur = x[0]; 
	double alpha = par[0]; 
	double n = par[1]; 
	double mu = par[2]; 
	double sigma = par[3]; 
	double N = par[4]; 
	TF1* exp = new TF1("exp","exp(x)",1e-20,1e20); 
	double A; double B; 
	if (alpha < 0)
	{ 
		A = pow((n/(-1*alpha)),n)*exp->Eval((-1)*alpha*alpha/2); 
		B = n/(-1*alpha) + alpha;} 
	else 
	{ 
		A = pow((n/alpha),n)*exp->Eval((-1)*alpha*alpha/2); 
		B = n/alpha - alpha;
	}

	double f; 
	if ((xcur-mu)/sigma > (-1)*alpha)     
	{
		f = N*exp->Eval((-1)*(xcur-mu)*(xcur-mu)/(2*sigma*sigma)); 
	}
	else 
	{
		f = N*A*pow((B- (xcur-mu)/sigma),(-1*n)); 
	}
	delete exp; 
	return f; 
} 




