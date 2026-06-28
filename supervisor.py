import streamlit as st  
import pandas as pd     
import os               
import time 
import json 
from datetime import datetime

# CONFIGURAÇÃO INICIAL
st.set_page_config(page_title="SUPERVISÓRIO BOMBEAMENTO", layout="wide")

ARQUIVO_LEITURAS = 'leituras.json' 
ARQUIVO_COMANDOS = 'comandos.json' 

if 'logado' not in st.session_state:
    st.session_state.logado = False
    st.session_state.perfil = None  
    st.session_state.usuario = ""

if 'historico_comandos' not in st.session_state:
    st.session_state.historico_comandos = []

# ==========================================================================
# 1. TELA DE LOGIN (Controle de Acesso)
# ==========================================================================
if not st.session_state.logado:
    st.title("🔒 Sistema de Supervisão - Autenticação Requerida")
    with st.form("tela_de_login"):
        user_input = st.text_input("Usuário")
        password_input = st.text_input("Senha", type="password")
        botao_entrar = st.form_submit_button("Entrar no Sistema")
        
        if botao_entrar:
            if user_input == "admin" and password_input == "admin123":
                st.session_state.logado, st.session_state.perfil, st.session_state.usuario = True, "admin", "Administrador"
                st.rerun()
            elif user_input == "operador" and password_input == "op123":
                st.session_state.logado, st.session_state.perfil, st.session_state.usuario = True, "operador", "Operador de Sala"
                st.rerun()
            else:
                st.error("Usuário ou senha incorretos!")
    st.stop()

# ==========================================================================
# 2. CABEÇALHO DO PAINEL
# ==========================================================================
col_tit, col_user = st.columns([4, 1])
with col_tit: st.title("🏭 Supervisório Industrial - Misturador")
with col_user:
    st.write(f"👤 **Usuário:** {st.session_state.usuario} ({st.session_state.perfil.upper()})")
    if st.button("🚪 Sair (Logout)"):
        st.session_state.logado = False
        st.rerun()

st.markdown("---")

# ==========================================================================
# 3. LEITURA E TRATAMENTO DE DADOS (CORRIGIDO)
# ==========================================================================
df_geral = pd.DataFrame()
erro_leitura = False
mensagem_erro = ""

try:
    if os.path.exists(ARQUIVO_LEITURAS):
        df_geral = pd.read_json(ARQUIVO_LEITURAS, lines=True)
        
        # NOVA LÓGICA DE FALHA: Checa apenas se o carimbo de tempo (timestamp) está corrompido,
        # pois campos vazios entre atuadores e sensores são normais na sua estrutura.
        if 'timestamp' in df_geral.columns:
            linhas_corrompidas = df_geral['timestamp'].isnull()
            if linhas_corrompidas.any():
                st.warning("⚠️ Aviso: Foram detectadas leituras sem carimbo de tempo (Leitura Inválida). O sistema descartou as falhas.")
                # Remove apenas as linhas que realmente vieram quebradas do C++
                df_geral = df_geral[~linhas_corrompidas] 
    else:
        erro_leitura = True
        mensagem_erro = "Arquivo 'leituras.json' não encontrado."
except Exception as e:
    erro_leitura = True
    mensagem_erro = f"Falha crítica na comunicação com o arquivo JSON: {e}"

# ==========================================================================
# 4. EXIBIÇÃO DO SUPERVISÓRIO
# ==========================================================================
if not erro_leitura and not df_geral.empty:
    
    # Prepara os dados isolando apenas quem é sensor para desenhar os gráficos
    df_sensores = df_geral[df_geral['sensor'].notna()].copy()
    df_sensores['timestamp'] = pd.to_datetime(df_sensores['timestamp'])
    df_grafico = df_sensores.pivot_table(index='timestamp', columns='sensor', values='valor', aggfunc='last')

    # --- MÉTRICAS PRINCIPAIS ---
    col1, col2, col3, col4, col5 = st.columns(5)
    def get_last(nome):
        if nome in df_grafico.columns: return round(float(df_grafico[nome].dropna().iloc[-1]), 2)
        return "---"

    val_mix_tt = get_last('TQ-MIX-TT')
    val_mix_lt = get_last('TQ-MIX-LT')

    with col1: st.metric("Resistência Quente", f"{get_last('RES-Q-TT')} °C")
    with col2: st.metric("Resfriador Frio", f"{get_last('RES-F-TT')} °C")
    with col3: st.metric("Tanque Misturador", f"{val_mix_tt} °C")
    with col4: st.metric("Nível do Tanque", f"{val_mix_lt} %")
    with col5: st.metric("Pressão Interna", f"{get_last('TQ-MIX-PT')} bar")

    # --- Tabela de leituras atuais e Histórico Consultável ---
    with st.expander("📋 Ver Tabela de Leituras Atuais (Dados Brutos)"):
        st.dataframe(df_geral.tail(20)) 

    # --- Gráfico histórico de pelo menos duas variáveis ---
    col_chart1, col_chart2 = st.columns(2)
    with col_chart1:
        st.subheader("📈 Temperaturas (°C)")
        temps = [c for c in ['RES-Q-TT', 'RES-F-TT', 'TQ-MIX-TT'] if c in df_grafico.columns]
        if temps: st.line_chart(df_grafico[temps])
    with col_chart2:
        st.subheader("📊 Nível e Pressão")
        outros = [c for c in ['TQ-MIX-LT', 'TQ-MIX-PT'] if c in df_grafico.columns]
        if outros: st.line_chart(df_grafico[outros])

    # --- Resumo do estado das bombas ou atuadores ---
    st.markdown("---")
    st.subheader("🔌 Estado dos Atuadores")
    c_bq, c_bf = st.columns(2)
    df_atuadores = df_geral[df_geral['tipo'] == 'atuador']
    
    def get_bomba(nome):
        filtro = df_atuadores[df_atuadores['nome'] == nome]
        if not filtro.empty: return filtro.iloc[-1]['estado'], filtro.iloc[-1]['valor']
        return "Desconectado", 0

    est_q, pot_q = get_bomba('BOMBA-Q')
    est_f, pot_f = get_bomba('BOMBA-F')

    with c_bq: st.info(f"**Bomba Quente (Q)** | Status: {est_q} | Carga: {pot_q}%")
    with c_bf: st.info(f"**Bomba Fria (F)** | Status: {est_f} | Carga: {pot_f}%")

    # ==========================================================================
    # 5. PAINEL DE COMANDOS (Envio para C++)
    # ==========================================================================
    st.markdown("---")
    st.subheader("🎛️ Painel de Operação")
    eh_operador = (st.session_state.perfil == "operador")

    def enviar_comando(setpoint, vazao, manual_q, manual_f, estop, nome_acao):
        dados_comando = {
            "setpoint_temp": float(setpoint), "vazao_alvo": float(vazao),
            "override_bomba_q": bool(manual_q), "override_bomba_f": bool(manual_f),
            "parada_emergencia": bool(estop), "operador_ativo": st.session_state.usuario
        }
        with open(ARQUIVO_COMANDOS, 'w') as f:
            json.dump(dados_comando, f, indent=4)
        
        hora_atual = datetime.now().strftime("%H:%M:%S")
        log = f"[{hora_atual}] {st.session_state.usuario}: {nome_acao}"
        st.session_state.historico_comandos.insert(0, log) 

    col_cmd1, col_cmd2, col_cmd3 = st.columns(3)
    
    with col_cmd1:
        with st.form("form_processo"):
            st.write("**Parâmetros de Processo**")
            setpoint_alvo = st.slider("Setpoint (°C)", 0.0, 100.0, 50.0, disabled=eh_operador)
            vazao_alvo = st.number_input("Vazão (L/min)", 0.0, 10.0, 2.0, disabled=eh_operador)
            if st.form_submit_button("Aplicar", disabled=eh_operador):
                enviar_comando(setpoint_alvo, vazao_alvo, False, False, False, f"Alterou Setpoint para {setpoint_alvo}°C")
                st.success("Comando enviado!")

    with col_cmd2:
        st.write("**Atuadores Manuais**")
        bomba_q = st.toggle("Ligar Bomba Q", disabled=eh_operador)
        bomba_f = st.toggle("Ligar Bomba F", disabled=eh_operador)
        if (bomba_q or bomba_f) and not eh_operador:
            enviar_comando(50.0, 2.0, bomba_q, bomba_f, False, "Acionamento manual de bomba(s)")

    with col_cmd3:
        st.write("**Emergência**")
        if st.button("🛑 PARADA DE EMERGÊNCIA", type="primary", use_container_width=True, disabled=eh_operador):
            enviar_comando(0.0, 0.0, False, False, True, "ACIONOU PARADA DE EMERGÊNCIA")
            st.error("Emergência acionada!")

    # ==========================================================================
    # 6. LISTA DE ALARMES E HISTÓRICO DE COMANDOS
    # ==========================================================================
    st.markdown("---")
    col_al, col_hist = st.columns(2)
    
    with col_al:
        st.subheader("🚨 Lista de Alarmes")
        alarmes_ativos = []
        if isinstance(val_mix_tt, (int, float)) and val_mix_tt > 75.0:
            alarmes_ativos.append("🔴 ALTA TEMPERATURA NO TANQUE MISTURADOR (>75°C)")
        if isinstance(val_mix_lt, (int, float)) and val_mix_lt > 90.0:
            alarmes_ativos.append("🔴 RISCO DE TRANSBORDAMENTO - Nível Alto (>90%)")
            
        if not alarmes_ativos:
            st.success("✅ Nenhum alarme ativo. Sistema operando normalmente.")
        else:
            for alarme in alarmes_ativos:
                st.error(alarme)

    with col_hist:
        st.subheader("📜 Histórico de Comandos")
        if len(st.session_state.historico_comandos) == 0:
            st.write("Nenhum comando enviado nesta sessão.")
        else:
            for cmd in st.session_state.historico_comandos[:5]:
                st.caption(cmd)

    time.sleep(5)
    st.rerun()

else:
    # Ajustado para exibir o motivo real caso o arquivo esteja vazio ou falhe
    if mensagem_erro:
        st.error(f"🚨 FALHA CRÍTICA: {mensagem_erro}")
    else:
        st.error("🚨 FALHA CRÍTICA: O arquivo de dados foi lido, mas a tabela está vazia. O C++ enviou algum dado?")
        
    st.warning("Verifique se o programa C++ está rodando e gerando o arquivo JSON corretamente.")
    if st.button("Tentar Reconectar"):
        st.rerun()