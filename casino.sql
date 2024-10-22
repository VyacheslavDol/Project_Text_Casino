--
-- PostgreSQL database dump
--

-- Dumped from database version 15.7 (Ubuntu 15.7-0ubuntu0.23.10.1)
-- Dumped by pg_dump version 15.7 (Ubuntu 15.7-0ubuntu0.23.10.1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: uuid-ossp; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS "uuid-ossp" WITH SCHEMA public;


--
-- Name: EXTENSION "uuid-ossp"; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION "uuid-ossp" IS 'generate universally unique identifiers (UUIDs)';


SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: mail; Type: TABLE; Schema: public; Owner: snuffy
--

CREATE TABLE public.mail (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    user_id uuid,
    link character varying(38),
    is_active boolean DEFAULT false
);


ALTER TABLE public.mail OWNER TO snuffy;

--
-- Name: profile; Type: TABLE; Schema: public; Owner: snuffy
--

CREATE TABLE public.profile (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    user_id uuid,
    balance character varying(64) DEFAULT 200
);


ALTER TABLE public.profile OWNER TO snuffy;

--
-- Name: token; Type: TABLE; Schema: public; Owner: snuffy
--

CREATE TABLE public.token (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    user_id uuid,
    refresh_token text
);


ALTER TABLE public.token OWNER TO snuffy;

--
-- Name: users; Type: TABLE; Schema: public; Owner: snuffy
--

CREATE TABLE public.users (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    login character varying(32) NOT NULL,
    email character varying(64) NOT NULL,
    password character varying(256) NOT NULL
);


ALTER TABLE public.users OWNER TO snuffy;

--
-- Name: mail mail_pkey; Type: CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.mail
    ADD CONSTRAINT mail_pkey PRIMARY KEY (id);


--
-- Name: profile profile_pkey; Type: CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.profile
    ADD CONSTRAINT profile_pkey PRIMARY KEY (id);


--
-- Name: token token_pkey; Type: CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.token
    ADD CONSTRAINT token_pkey PRIMARY KEY (id);


--
-- Name: users unique_email; Type: CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT unique_email UNIQUE (email);


--
-- Name: users unique_login; Type: CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT unique_login UNIQUE (login);


--
-- Name: users users_pkey; Type: CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_pkey PRIMARY KEY (id);


--
-- Name: mail mail_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.mail
    ADD CONSTRAINT mail_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id) ON DELETE CASCADE;


--
-- Name: profile profile_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.profile
    ADD CONSTRAINT profile_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id);


--
-- Name: token token_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: snuffy
--

ALTER TABLE ONLY public.token
    ADD CONSTRAINT token_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id) ON DELETE CASCADE;


--
-- Name: SCHEMA public; Type: ACL; Schema: -; Owner: pg_database_owner
--

GRANT ALL ON SCHEMA public TO snuffy;


--
-- PostgreSQL database dump complete
--

