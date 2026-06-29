import streamlit as st
import pandas as pd
import os
import time
import json
from datetime import datetime

st.set_page_config(
    page_title="Mini-SCADA EB-61",
    layout="wide",
    initial_sidebar_state="collapsed"
)

ARQUIVO_LEITURAS = 'leituras.json'
ARQUIVO_COMANDOS = 'comandos.json'
CSV_HISTORICO    = 'historico.csv'

st.markdown("""
<style>
    body, .main, .block-container {
        background-color: #1a1a2e !important;
        color: #e0e0e0 !important;
        padding-top: 0.5rem !important;
        padding-bottom: 0.5rem !important;
    }
    .block-container { padding: 0.5rem 1rem !important; max-width: 100% !important; }
    h1, h2, h3 { color: #00d4ff !important; margin: 0 !important; padding: 0 !important; }
    .stMetric {
        background-color: #16213e;
        border: 1px solid #0f3460;
        border-radius: 6px;
        padding: 6px !important;
    }
    .stMetric label { color: #aaaaaa !important; font-size: 0.75rem !important; }
    .stMetric [data-testid="stMetricValue"] { color: #00ff88 !important; font-size: 1.1rem !important; font-family: monospace; }
    .card-ok {
        background-color: #0a2a1a;
        border: 1px solid #00aa44;
        border-radius: 6px;
        padding: 8px;
        text-align: center;
        color: #00ff88;
        font-family: monospace;
        font-size: 0.85rem;
    }
    .card-alarme {
        background-color: #2a0a0a;
        border: 2px solid #ff4444;
        border-radius: 6px;
        padding: 8px;
        text-align: center;
        color: #ff4444;
        font-family: monospace;
        font-size: 0.85rem;
        animation: pisca 1s infinite;
    }
    .card-atencao {
        background-color: #2a1a00;
        border: 1px solid #ffaa00;
        border-radius: 6px;
        padding: 8px;
        text-align: center;
        color: #ffaa00;
        font-family: monospace;
        font-size: 0.85rem;
    }
    @keyframes pisca { 0%,100% { opacity:1; } 50% { opacity:0.5; } }
    .secao-titulo {
        background-color: #0f3460;
        color: #00d4ff;
        padding: 3px 8px;
        border-radius: 4px;
        font-size: 0.75rem;
        font-weight: bold;
        letter-spacing: 1px;
        margin-bottom: 4px;
    }
    .alarme-linha {
        background-color: #2a0a0a;
        border-left: 4px solid #ff4444;
        color: #ff8888;
        padding: 3px 8px;
        margin: 2px 0;
        font-family: monospace;
        font-size: 0.8rem;
        border-radius: 0 4px 4px 0;
    }
    .normal-linha {
        background-color: #0a2a1a;
        border-left: 4px solid #00aa44;
        color: #00ff88;
        padding: 3px 8px;
        font-family: monospace;
        font-size: 0.8rem;
        border-radius: 0 4px 4px 0;
    }
    div[data-testid="stButton"] button {
        background-color: #0f3460;
        color: #00d4ff;
        border: 1px solid #00d4ff;
        font-size: 0.8rem;
        padding: 4px 12px;
    }
    .stSlider { padding: 0 !important; }
    hr { border-color: #0f3460 !important; margin: 4px 0 !important; }
    .header-bar {
        background: linear-gradient(90deg, #0f3460, #16213e);
        border-bottom: 2px solid #00d4ff;
        padding: 6px 12px;
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin-bottom: 8px;
        border-radius: 4px;
    }
</style>
""", unsafe_allow_html=True)


# ESTADO DE SESSÃO
if 'logado' not in st.session_state:
    st.session_state.logado   = False
    st.session_state.perfil   = None
    st.session_state.usuario  = ""
if 'historico_comandos' not in st.session_state:
    st.session_state.historico_comandos = []

# LOGIN
if not st.session_state.logado:
    st.markdown("""
    <div style='text-align:center; padding: 60px 0 20px 0;'>
        <h1 style='color:#00d4ff; font-size:2rem;'>🏭 Mini-SCADA — Planta de Mistura</h1>
        <p style='color:#aaa;'>Estação EB-61 | Sistema de Supervisão e Controle</p>
    </div>
    """, unsafe_allow_html=True)

    col_login = st.columns([1, 2, 1])[1]
    with col_login:
        with st.form("login"):
            st.markdown("#### Autenticação Requerida")
            usuario  = st.text_input("Usuário")
            senha    = st.text_input("Senha", type="password")
            entrar   = st.form_submit_button("Entrar", use_container_width=True)
            if entrar:
                credenciais = {
                    "admin":    ("admin123",  "admin",    "Administrador"),
                    "tecnico":  ("tec456",    "tecnico",  "Técnico"),
                    "operador": ("op789",     "operador", "Operador"),
                }
                if usuario in credenciais and senha == credenciais[usuario][0]:
                    st.session_state.logado  = True
                    st.session_state.perfil  = credenciais[usuario][1]
                    st.session_state.usuario = credenciais[usuario][2]
                    st.rerun()
                else:
                    st.error("Usuário ou senha incorretos.")
    st.stop()

# LEITURA DO JSON
df_geral = pd.DataFrame()
erro_msg = ""

try:
    if os.path.exists(ARQUIVO_LEITURAS):
        df_geral = pd.read_json(ARQUIVO_LEITURAS, lines=True)
        if 'timestamp' in df_geral.columns:
            df_geral = df_geral[df_geral['timestamp'].notna()]
    else:
        erro_msg = "leituras.json não encontrado. O C++ está rodando?"
except Exception as e:
    erro_msg = f"Erro ao ler JSON: {e}"

# Salva CSV
if not df_geral.empty:
    try:
        if os.path.exists(CSV_HISTORICO):
            df_hist = pd.concat([pd.read_csv(CSV_HISTORICO), df_geral]).drop_duplicates()
        else:
            df_hist = df_geral
        df_hist.to_csv(CSV_HISTORICO, index=False)
    except:
        pass

# PREPARA DADOS
df_pivot     = pd.DataFrame()
df_atuadores = pd.DataFrame()
df_alarmes   = pd.DataFrame()

if not df_geral.empty:
    df_sensores = df_geral[df_geral['sensor'].notna()].copy()
    if not df_sensores.empty:
        df_sensores['timestamp'] = pd.to_datetime(df_sensores['timestamp'])
        df_pivot = df_sensores.pivot_table(
            index='timestamp', columns='sensor', values='valor', aggfunc='last'
        )
    df_atuadores = df_geral[df_geral['tipo'] == 'atuador'].copy()
    df_alarmes   = df_geral[df_geral['tipo'] == 'alarme'].copy()

def ultimo(sensor, decimais=1):
    if not df_pivot.empty and sensor in df_pivot.columns:
        s = df_pivot[sensor].dropna()
        if not s.empty:
            return round(float(s.iloc[-1]), decimais)
    return None

def fmt(val, unidade="", decimais=1):
    if val is None: return "---"
    return f"{val:.{decimais}f} {unidade}".strip()

temp_mix = ultimo('TQ-MIX-TT')
nivel    = ultimo('TQ-MIX-LT')
pressao  = ultimo('TQ-MIX-PT')
temp_q   = ultimo('RES-Q-TT')
temp_f   = ultimo('RES-F-TT')

def card(titulo, valor, unidade, ok_min=None, ok_max=None):
    if valor is None:
        classe = "card-atencao"
        display = "---"
    else:
        fora = (ok_min is not None and valor < ok_min) or \
               (ok_max is not None and valor > ok_max)
        classe = "card-alarme" if fora else "card-ok"
        display = fmt(valor, unidade)
    return f"""<div class='{classe}'>
        <div style='font-size:0.7rem;color:#aaa;'>{titulo}</div>
        <div style='font-size:1.1rem;font-weight:bold;'>{display}</div>
    </div>"""

def info_bomba(nome):
    if not df_atuadores.empty:
        f = df_atuadores[df_atuadores['nome'] == nome]
        if not f.empty:
            return f.iloc[-1].get('estado','---'), f.iloc[-1].get('valor', 0)
    return "---", 0

est_q, vaz_q = info_bomba('BOMBA-Q')
est_f, vaz_f = info_bomba('BOMBA-F')

# Detecta alarmes
alarmes_ativos = []
if temp_mix is not None and temp_mix > 46.0: alarmes_ativos.append("TEMP MISTURA ACIMA DO SETPOINT")
if temp_mix is not None and temp_mix < 44.0: alarmes_ativos.append("TEMP MISTURA ABAIXO DO SETPOINT")
if nivel    is not None and nivel    >= 82.5: alarmes_ativos.append("NIVEL ALTO NO TANQUE")
if nivel    is not None and nivel    <= 27.5: alarmes_ativos.append("NIVEL BAIXO NO TANQUE")
if pressao  is not None and pressao  >= 6.5:  alarmes_ativos.append("PRESSAO ALTA NA SAIDA")
if temp_q   is not None and temp_q   < 55.0:  alarmes_ativos.append("FALHA SERPENTINA QUENTE")
if temp_f   is not None and temp_f   > 30.0:  alarmes_ativos.append("FALHA SERPENTINA FRIA")
if not df_alarmes.empty:
    for _, row in df_alarmes.tail(3).iterrows():
        cod = str(row.get('codigo',''))
        if cod and cod not in alarmes_ativos:
            alarmes_ativos.append(f"[C++] {cod}")

# CABEÇALHO
agora = datetime.now().strftime("%d/%m/%Y  %H:%M:%S")
perfil_txt = st.session_state.perfil.upper()
usuario_txt = st.session_state.usuario

st.markdown(f"""
<div class='header-bar'>
    <div>
        <span style='color:#00d4ff;font-size:1.1rem;font-weight:bold;'>
            🏭 Mini-SCADA — Planta de Mistura &nbsp;|&nbsp;
            <span style='color:#ffaa00;'>EB-61</span>
        </span>
    </div>
    <div style='color:#aaa;font-size:0.85rem;font-family:monospace;'>
        🕐 {agora} &nbsp;&nbsp; 👤 {usuario_txt} [{perfil_txt}]
    </div>
</div>
""", unsafe_allow_html=True)

if st.session_state.get('mostrar_sair'):
    if st.button("Confirmar Logout"):
        st.session_state.logado = False
        st.rerun()

# LINHA 1: RESERVATÓRIOS | TANQUE | BOMBAS
col_res, col_tanque, col_bombas = st.columns([2, 3, 2])

with col_res:
    st.markdown("<div class='secao-titulo'>⚗️ RESERVATÓRIOS</div>", unsafe_allow_html=True)
    st.markdown(card("RES-Q-TT  Temp. Quente", temp_q, "°C", 55.0, 75.0), unsafe_allow_html=True)
    st.markdown("<div style='margin:4px'></div>", unsafe_allow_html=True)
    st.markdown(card("RES-F-TT  Temp. Fria",   temp_f, "°C", 10.0, 30.0), unsafe_allow_html=True)
    st.markdown("<div style='margin:4px'></div>", unsafe_allow_html=True)
    serp_q = "🟢 NORMAL" if temp_q and temp_q >= 55.0 else "🔴 FALHA"
    serp_f = "🟢 NORMAL" if temp_f and temp_f <= 30.0 else "🔴 FALHA"
    st.markdown(f"""
    <div style='background:#16213e;border:1px solid #0f3460;border-radius:6px;padding:6px;font-size:0.75rem;font-family:monospace;'>
        <div>🔥 Serpentina Quente: <b>{serp_q}</b></div>
        <div>❄️ Serpentina Fria: &nbsp;&nbsp;<b>{serp_f}</b></div>
    </div>""", unsafe_allow_html=True)

with col_tanque:
    st.markdown("<div class='secao-titulo'>🏺 TANQUE DE MISTURA — TQ-MIX</div>", unsafe_allow_html=True)
    c1, c2, c3 = st.columns(3)
    with c1: st.markdown(card("TQ-MIX-TT  Temperatura", temp_mix, "°C", 44.0, 46.0), unsafe_allow_html=True)
    with c2: st.markdown(card("TQ-MIX-LT  Nível",       nivel,    "%",  27.5, 82.5), unsafe_allow_html=True)
    with c3: st.markdown(card("TQ-MIX-PT  Pressão",     pressao,  "BAR",None, 6.5),  unsafe_allow_html=True)

    st.markdown("<div style='margin:6px'></div>", unsafe_allow_html=True)

    # Barra visual de nível
    nivel_val = nivel if nivel is not None else 0
    cor_nivel = "#ff4444" if nivel_val >= 82.5 or nivel_val <= 27.5 else "#00ff88"
    st.markdown(f"""
    <div style='background:#0f1923;border:1px solid #0f3460;border-radius:4px;padding:4px 8px;'>
        <div style='font-size:0.7rem;color:#aaa;margin-bottom:2px;'>NÍVEL VISUAL</div>
        <div style='background:#111;border-radius:4px;height:14px;'>
            <div style='background:{cor_nivel};width:{nivel_val}%;height:14px;border-radius:4px;'></div>
        </div>
        <div style='display:flex;justify-content:space-between;font-size:0.65rem;color:#555;'>
            <span>0%</span><span>▲27.5%</span><span>50%</span><span>▲82.5%</span><span>100%</span>
        </div>
    </div>""", unsafe_allow_html=True)

    st.markdown("<div style='margin:4px'></div>", unsafe_allow_html=True)

    # Mini gráfico de temperatura
    if not df_pivot.empty:
        colunas = [c for c in ['TQ-MIX-TT','RES-Q-TT','RES-F-TT'] if c in df_pivot.columns]
        if colunas:
            st.markdown("<div style='font-size:0.7rem;color:#aaa;'>📈 Histórico Temperaturas</div>", unsafe_allow_html=True)
            st.line_chart(df_pivot[colunas].tail(30), height=100, use_container_width=True)

with col_bombas:
    st.markdown("<div class='secao-titulo'>⚙️ BOMBAS E VÁLVULAS</div>", unsafe_allow_html=True)
    cor_bq = "card-ok" if est_q in ["PRINCIPAL","RESERVA"] else "card-atencao"
    cor_bf = "card-ok" if est_f in ["PRINCIPAL","RESERVA"] else "card-atencao"
    st.markdown(f"""
    <div class='{cor_bq}'>
        <div style='font-size:0.7rem;color:#aaa;'>BOMBA-Q1/Q2 (Linha Quente)</div>
        <div>Status: <b>{est_q}</b> | {fmt(vaz_q,'L/min',1)}</div>
    </div>""", unsafe_allow_html=True)
    st.markdown("<div style='margin:4px'></div>", unsafe_allow_html=True)
    st.markdown(f"""
    <div class='{cor_bf}'>
        <div style='font-size:0.7rem;color:#aaa;'>BOMBA-F1/F2 (Linha Fria)</div>
        <div>Status: <b>{est_f}</b> | {fmt(vaz_f,'L/min',1)}</div>
    </div>""", unsafe_allow_html=True)
    st.markdown("<div style='margin:4px'></div>", unsafe_allow_html=True)

    # Mini gráfico nível
    if not df_pivot.empty and 'TQ-MIX-LT' in df_pivot.columns:
        st.markdown("<div style='font-size:0.7rem;color:#aaa;'>📊 Histórico Nível</div>", unsafe_allow_html=True)
        st.line_chart(df_pivot[['TQ-MIX-LT']].tail(30), height=80, use_container_width=True)

# LINHA 2: COMANDOS | ALARMES
st.markdown("<hr>", unsafe_allow_html=True)
col_cmd, col_alarm = st.columns([3, 2])

with col_cmd:
    st.markdown("<div class='secao-titulo'>🎛️ PAINEL DE COMANDOS</div>", unsafe_allow_html=True)
    eh_operador = st.session_state.perfil == "operador"
    eh_admin    = st.session_state.perfil == "admin"

    def enviar_comando(setpoint, vazao, mq, mf, estop, desc):
        dados = {
            "setpoint_temp":    float(setpoint),
            "vazao_alvo":       float(vazao),
            "override_bomba_q": bool(mq),
            "override_bomba_f": bool(mf),
            "parada_emergencia":bool(estop),
            "operador_ativo":   st.session_state.usuario
        }
        with open(ARQUIVO_COMANDOS, 'w') as f:
            json.dump(dados, f, indent=4)
        hora = datetime.now().strftime("%H:%M:%S")
        st.session_state.historico_comandos.insert(
            0, f"[{hora}] {st.session_state.usuario}: {desc}"
        )

    cc1, cc2, cc3 = st.columns([3, 2, 2])

    with cc1:
        with st.form("form_sp"):
            sp = st.slider("Setpoint Temperatura (°C)", 21.0, 69.0, 45.0, step=0.5)
            vz = st.number_input("Volume (L/min)", 10.0, 100.0, 60.0, step=5.0)
            if st.form_submit_button("✅ Aplicar Setpoint", use_container_width=True):
                enviar_comando(sp, vz, False, False, False, f"Setpoint → {sp}°C | {vz} L/min")
                st.success("Enviado!")

    with cc2:
        st.markdown("<div style='font-size:0.75rem;color:#aaa;'>Atuadores Manuais</div>", unsafe_allow_html=True)
        mq = st.toggle("Bomba Quente", disabled=eh_operador)
        mf = st.toggle("Bomba Fria",   disabled=eh_operador)
        if st.button("Aplicar", disabled=eh_operador, use_container_width=True):
            enviar_comando(45.0, 60.0, mq, mf, False, f"Manual: BQ={mq} BF={mf}")
            st.success("Enviado!")

    with cc3:
        st.markdown("<div style='font-size:0.75rem;color:#aaa;'>Segurança</div>", unsafe_allow_html=True)
        st.markdown("<div style='margin:8px'></div>", unsafe_allow_html=True)
        if st.button("🛑 EMERGÊNCIA", type="primary",
                     use_container_width=True, disabled=not eh_admin):
            enviar_comando(0.0, 0.0, False, False, True, "PARADA DE EMERGÊNCIA")
            st.error("Emergência acionada!")
        st.markdown("<div style='margin:4px'></div>", unsafe_allow_html=True)
        if st.button("🚪 Logout", use_container_width=True):
            st.session_state.logado = False
            st.rerun()

with col_alarm:
    st.markdown("<div class='secao-titulo'>🚨 ALARMES E HISTÓRICO</div>", unsafe_allow_html=True)

    if alarmes_ativos:
        for a in alarmes_ativos:
            st.markdown(f"<div class='alarme-linha'>🔴 {a}</div>", unsafe_allow_html=True)
    else:
        st.markdown("<div class='normal-linha'>✅ Sistema operando normalmente</div>", unsafe_allow_html=True)

    st.markdown("<div style='margin:6px'></div>", unsafe_allow_html=True)
    st.markdown("<div style='font-size:0.7rem;color:#aaa;'>Últimos comandos:</div>", unsafe_allow_html=True)
    if st.session_state.historico_comandos:
        for cmd in st.session_state.historico_comandos[:5]:
            st.markdown(f"<div style='font-size:0.72rem;color:#888;font-family:monospace;padding:1px 0;'>{cmd}</div>",
                        unsafe_allow_html=True)
    else:
        st.markdown("<div style='font-size:0.72rem;color:#555;'>Nenhum comando nesta sessão.</div>",
                    unsafe_allow_html=True)

# ATUALIZAÇÃO AUTOMÁTICA
time.sleep(5)
st.rerun()