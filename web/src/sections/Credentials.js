import React, { Component } from 'react';
import {Input, Password} from "../modules/Input.js"

export default class Credentials extends Component {
	constructor(props) {
		super(props);

		this.state = {display: true, r_username: "", r_fullname: "", r_password: "", s_username: "", s_password: "", show_register: false, processing: false};
	}

	handleErrors = (response) => {
		if (!response.ok) {
			throw Error(response.text());
		}
		return response.text();
	}

	componentWillMount() {
		// If session exists...
		// Login
		fetch(this.props.domain + "checkAuth")
		.then(this.handleErrors)
		.then((e) => {
			if (e !== "__DEVE_INIT") {
				this.props.setUserInfo({
					username: e
				});
				this.hide();
			}
		}).catch((e) => {
			alert(e);
			console.log(e);
		});
	}

	handleSignUpUsername = (event) => {
		this.setState({r_username: event.target.value});
	}

	handleSignUpFullname = (event) => {
		this.setState({r_fullname: event.target.value});
	}

	handleSignUpPassword = (event) => {
		this.setState({r_password: event.target.value});
	}

	handleSignInUsername = (event) => {
		this.setState({s_username: event.target.value});
	}

	handleSignInPassword = (event) => {
		this.setState({s_password: event.target.value});
	}

	setSignState = (toggle) => {
		this.setState({show_register: toggle});
	}

	hide = () => {
		this.setState({processing: false, display: false});
	}

	handleSignIn = (e) => {
		e.preventDefault();
		this.setState({processing: true});

		if (this.state.s_username === "" || this.state.s_password === "") {
			this.setState({processing: false, r_error: true});
			return;
		}
		
		let b = this.state.s_username + ";" + this.state.s_password;

		fetch(this.props.domain + "signin", {
			method: "POST",
			body: b
		})
		.then(this.handleErrors)
		.then((e) => {
			console.log(e);
			
			this.setState({processing: false, r_error: false});
			this.props.setUserInfo({
				username: this.state.s_username
			});
			this.hide();
		}).catch((e) => {
			alert(e);
			console.log(e);
			
			this.setState({processing: false, r_error: true});
		});
	}

	handleSignUp = (e) => {
		e.preventDefault();
		this.setState({processing: true});

		if (this.state.r_username === "" || this.state.r_password === "") {
			this.setState({processing: false, r_error: true});
			return;
		}
		
		let b = this.state.r_username + ";" + this.state.r_password;

		let me = this;

		fetch(this.props.domain + "signup", {
			method: "POST",
			body: b
		})
		.then(this.handleErrors)
		.then((e) => {
			console.log(e);
			
			me.setState({processing: false, r_error: false});
			me.props.setUserInfo({
				username: this.state.r_username
			});
			me.hide();
		}).catch((e) => {
			console.log(e);
			alert(e);
			
			me.setState({processing: false, r_error: true});
		});
	}
	
	logOut = () => {
		this.setState({r_fullname: "", r_username: "", r_password: "", s_username: "", s_password: "", display: true});
	}

	render() {
		return (
            <section className={`credentials${this.state.display ? "" : " hide"}`}>
				<div className="side"></div>
                <div className="main">
					<fieldset disabled={this.state.processing}>
						<div className={`credentials-container${this.state.show_register ? " hide" : ""}`}>
							<form onSubmit={this.handleSignUp}>
								<h1>Sign up with <span><span>D</span>EVE</span></h1>
								<div className="tagline">Have an account? <span onClick={() => this.setSignState(true)}>Sign In</span></div>
								<Input label="Username" placeholder="Username" value={this.state.r_username} onChange={this.handleSignUpUsername} type="text" />
								<Password label="Password" placeholder="Password" value={this.state.r_password} onChange={this.handleSignUpPassword} type="password" />
								<input type="submit" value="Sign In" />
							</form>
						</div>
						<div className={`credentials-container${this.state.show_register ? "" : " hide"}`}>
							<form onSubmit={this.handleSignIn}>
								<h1>Sign in to <span><span>D</span>EVE</span></h1>
								<div className="tagline">Don't have an account? <span onClick={() => this.setSignState(false)}>Sign Up</span></div>
								<Input label="Username" placeholder="Username" value={this.state.s_username} onChange={this.handleSignInUsername} type="text" />
								<Password label="Password" placeholder="Password" value={this.state.s_password} onChange={this.handleSignInPassword} type="password" />
								<input type="submit" value="Sign In" />
							</form>
						</div>
					</fieldset>
				</div>
            </section>
		);
	}
}