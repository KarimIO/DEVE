import React, { Component } from 'react';

export default class MyImages extends Component {
	render() {
		let empty = this.props.gallery.length === 0;
		return (
			<section className={`main-section${this.props.loading ? " loading" : ""}${empty ? " empty":""}`}>
				<div className="gallery">
					{this.props.gallery.map((e, i) => {
						return <div key={i} style={{backgroundImage: "url(data:image/jpeg;base64,"+e.thumb+")"}}>
							<div>
								<span>
									{e.access > 0 && <i>{e.access} view{e.access !== 1 && "s"} left</i>}
									{e.access === 0 && <button onClick={(ev) => this.props.requestImg(e.object)}>Request</button>}
									{e.access > 0 && <button onClick={(ev) => this.props.showImageView(e.object)}>View</button>}
								</span>
							</div>
						</div>;
					})}
				</div>
			</section>
		);
	}
}